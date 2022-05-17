/**
 *
 *  UserCtrl.cpp
 *  This file is generated by drogon_ctl
 *
 */

#include "UserCtrl.h"

#include <string>
#include "bcrypt/bcrypt.h"
#include "jwt/JWT.h"
#include "uuid/UtilUUID.h"

namespace api::v1
{

	void User::login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
	{
		auto jsonPtr = req->getJsonObject();

		if (!jsonPtr || !jsonPtr->isMember("account") || !jsonPtr->isMember("pwd"))
		{
			Json::Value resultJson;
			resultJson["code"] = k400BadRequest;
			resultJson["msg"] = "Missing account or password.";
			return callback(HttpResponse::newHttpJsonResponse(resultJson));
		}

		auto callbackPtr =
				std::make_shared<std::function<void(const HttpResponsePtr &)>>(
						std::move(callback));
		auto dbClientPtr = getDbClient();
		Mapper<Users> mapper(dbClientPtr);

		mapper.findOne(
				Criteria(Users::Cols::_account, CompareOperator::EQ, (*jsonPtr)["account"]),
				[req, jsonPtr, callbackPtr, this](const Users &user)
				{
					if (!Utils::Bcrypt::compare((*jsonPtr)["pwd"].asString(), user.getPwd()->data()))
					{
						// Compare password hash.
						Json::Value resultJson;
						resultJson["code"] = k403Forbidden;
						resultJson["msg"] = "Incorrect account or password.";
						return (*callbackPtr)(HttpResponse::newHttpJsonResponse(resultJson));
					}

					try
					{
						auto userJson = user.toJson();
						userJson.removeMember("pwd");
						auto member = jsonPtr->isMember("member") && (*jsonPtr)["member"].asBool();

						Utils::jwt::JWT jwtGenerated = Utils::jwt::JWT::generateToken(
								{
										{"userid", picojson::value(userJson["id"].asInt64())},
										{"member", picojson::value(member)},
								},
								member);

						auto token = jwtGenerated.getToken();
						auto uuid = Utils::uuid::genUUID();

						auto redisClientPtr = getRedisClient();
						auto expiresAt = member ? jwtGenerated.getExpiration() : 3600;

						redisClientPtr->newTransactionAsync(
								[uuid, token, expiresAt, userJson, callbackPtr](const nosql::RedisTransactionPtr &transPtr)
								{
									auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + userJson["id"].asString();

									transPtr->execCommandAsync(
											[](const nosql::RedisResult &r) { /* this command works */ },
											[](const std::exception &err) { /* this command failed */ },
											"del %s", auth.data());

									transPtr->execCommandAsync(
											[](const nosql::RedisResult &r) { /* this command works */ },
											[](const std::exception &err) { /* this command failed */ },
											"hset %s %s %s", auth.data(), uuid.data(), token.data());

									transPtr->execCommandAsync(
											[](const nosql::RedisResult &r) { /* this command works */ },
											[](const std::exception &err) { /* this command failed */ },
											"expire %s %lld", auth.data(), expiresAt);

									transPtr->execute(
											[uuid, userJson, expiresAt, callbackPtr](const nosql::RedisResult &r)
											{
												Json::Value resultJson;
												resultJson["code"] = k200OK;
												resultJson["token"] = uuid;
												resultJson["data"] = userJson;
												resultJson["expiresAt"] = expiresAt;
												return (*callbackPtr)(HttpResponse::newHttpJsonResponse(resultJson));
											},
											[callbackPtr](const std::exception &err)
											{
												LOG_ERROR << err.what();
												Json::Value ret;
												ret["code"] = k500InternalServerError;
												ret["msg"] = err.what();
												auto resp = HttpResponse::newHttpJsonResponse(ret);
												resp->setStatusCode(k500InternalServerError);
												return (*callbackPtr)(resp);
											});
								});
					}
					catch (const std::exception &e)
					{
						LOG_ERROR << e.what();
						Json::Value ret;
						ret["code"] = k500InternalServerError;
						ret["msg"] = e.what();
						auto resp = HttpResponse::newHttpJsonResponse(ret);
						resp->setStatusCode(k500InternalServerError);
						return (*callbackPtr)(resp);
					}
				},
				[callbackPtr](const DrogonDbException &e)
				{
					const orm::UnexpectedRows *s = dynamic_cast<const orm::UnexpectedRows *>(&e.base());
					if (s)
					{
						auto resp = HttpResponse::newHttpResponse();
						resp->setStatusCode(k404NotFound);
						return (*callbackPtr)(resp);
					}
					LOG_ERROR << e.base().what();
					Json::Value resultJson;
					resultJson["code"] = k500InternalServerError;
					resultJson["msg"] = "database error.";
					return (*callbackPtr)(HttpResponse::newHttpJsonResponse(resultJson));
				});
	}

	void User::logout(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
	{
		auto userID = req->getAttributes()->get<std::string>("jwt_userid");
		auto redisClientPtr = getRedisClient();
		auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + userID;

		redisClientPtr->execCommandAsync(
				[callback](const nosql::RedisResult &r)
				{
					Json::Value resultJson;
					resultJson["code"] = k200OK;
					resultJson["msg"] = "Sign out successfully.";
					return callback(HttpResponse::newHttpJsonResponse(resultJson));
				},
				[callback](const std::exception &err)
				{
					LOG_ERROR << err.what();
					Json::Value resultJson;
					resultJson["code"] = k500InternalServerError;
					resultJson["msg"] = err.what();
					return callback(HttpResponse::newHttpJsonResponse(resultJson));
				},
				"del %s", auth.data());
	}

	void User::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
	{
		auto jsonPtr = req->getJsonObject();
		if (!jsonPtr || !jsonPtr->isMember("account") || !jsonPtr->isMember("pwd"))
		{
			Json::Value resultJson;
			resultJson["code"] = k400BadRequest;
			resultJson["msg"] = "Missing account or password.";
			return callback(HttpResponse::newHttpJsonResponse(resultJson));
		}

		Users object = Users(*jsonPtr);
		auto now = trantor::Date::now();
		object.setRegistime(now);
		object.setUpdatetime(now);

		auto dbClientPtr = getDbClient();
		Mapper<Users> mapper(dbClientPtr);
		auto callbackPtr =
				std::make_shared<std::function<void(const HttpResponsePtr &)>>(
						std::move(callback));

		mapper.insert(
				object,
				[req, jsonPtr, callbackPtr, this](Users user)
				{
					auto userJson = user.toJson();
					userJson.removeMember("pwd");
					auto member = jsonPtr->isMember("member") && (*jsonPtr)["member"].asBool();

					Utils::jwt::JWT jwtGenerated = Utils::jwt::JWT::generateToken(
							{
									{"userid", picojson::value(userJson["id"].asInt64())},
									{"member", picojson::value(member)},
							},
							member);

					auto token = jwtGenerated.getToken();
					auto uuid = Utils::uuid::genUUID();
					auto expiresAt = member ? jwtGenerated.getExpiration() : 3600;

					auto redisClientPtr = getRedisClient();
					auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + userJson["id"].asString();

					redisClientPtr->newTransactionAsync(
							[uuid, auth, token, expiresAt, userJson, callbackPtr](const nosql::RedisTransactionPtr &transPtr)
							{
								transPtr->execCommandAsync(
										[](const nosql::RedisResult &r) {},
										[](const std::exception &err) {},
										"hset %s %s %s", auth.data(), uuid.data(), token.data());

								transPtr->execCommandAsync(
										[](const nosql::RedisResult &r) {},
										[](const std::exception &err) {},
										"expire %s %lld", auth.data(), expiresAt);

								transPtr->execute(
										[uuid, userJson, callbackPtr](const nosql::RedisResult &r)
										{
											Json::Value resultJson;
											resultJson["code"] = k200OK;
											resultJson["token"] = uuid;
											resultJson["data"] = userJson;
											return (*callbackPtr)(HttpResponse::newHttpJsonResponse(resultJson));
										},
										[callbackPtr](const std::exception &err)
										{
											LOG_ERROR << err.what();
											Json::Value resultJson;
											resultJson["code"] = k500InternalServerError;
											resultJson["msg"] = err.what();
											auto resp = HttpResponse::newHttpJsonResponse(resultJson);
											resp->setStatusCode(k500InternalServerError);
											return (*callbackPtr)(resp);
										});
							});
				},
				[callbackPtr](const DrogonDbException &err)
				{
					const orm::UniqueViolation *s = dynamic_cast<const orm::UniqueViolation *>(&err.base());
					Json::Value resultJson;
					if (s)
					{
						resultJson["code"] = k409Conflict;
						resultJson["msg"] = "The account already exists.";
						auto resp = HttpResponse::newHttpJsonResponse(resultJson);
						resp->setStatusCode(k409Conflict);
						return (*callbackPtr)(resp);
					}

					LOG_ERROR << err.base().what();
					resultJson["code"] = k500InternalServerError;
					resultJson["msg"] = "database error";
					auto resp = HttpResponse::newHttpJsonResponse(resultJson);
					resp->setStatusCode(k500InternalServerError);
					return (*callbackPtr)(resp);
				});
	}

	void User::deleteOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
	{
		auto userID = req->getAttributes()->get<int64_t>("jwt_userid");
		auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + std::to_string(userID);

		auto dbClientPtr = getDbClient();
		auto callbackPtr =
				std::make_shared<std::function<void(const HttpResponsePtr &)>>(
						std::move(callback));
		drogon::orm::Mapper<Users> mapper(dbClientPtr);
		mapper.deleteByPrimaryKey(
				userID,
				[auth, callbackPtr, this](const size_t count)
				{
					if (count == 1)
					{
						// successed
						auto redisClientPtr = getRedisClient();
						redisClientPtr->execCommandAsync(
								[callbackPtr](const nosql::RedisResult &r) {},
								[callbackPtr](const std::exception &err)
								{
									LOG_ERROR << err.what();
								},
								"del %s", auth.data());

						Json::Value resultJson;
						resultJson["code"] = k200OK;
						resultJson["msg"] = "Successfully.";
						return (*callbackPtr)(HttpResponse::newHttpJsonResponse(resultJson));
					}
					else if (count == 0)
					{
						Json::Value ret;
						ret["code"] = k404NotFound;
						ret["msg"] = "No resources deleted";
						auto resp = HttpResponse::newHttpJsonResponse(ret);
						resp->setStatusCode(k404NotFound);
						(*callbackPtr)(resp);
					}
					else
					{
						LOG_FATAL << "Delete more than one records: " << count;
						Json::Value ret;
						ret["code"] = k500InternalServerError;
						ret["msg"] = "Database error";
						auto resp = HttpResponse::newHttpJsonResponse(ret);
						resp->setStatusCode(k500InternalServerError);
						return (*callbackPtr)(resp);
					}
				},
				[callbackPtr](const DrogonDbException &e)
				{
					LOG_ERROR << e.base().what();
					Json::Value ret;
					ret["code"] = k500InternalServerError;
					ret["msg"] = "database error";
					auto resp = HttpResponse::newHttpJsonResponse(ret);
					resp->setStatusCode(k500InternalServerError);
					return (*callbackPtr)(resp);
				});
	}

	void User::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
	{
		auto jsonPtr = req->jsonObject();
		auto id = req->getAttributes()->get<int64_t>("jwt_userid");
		if (!jsonPtr)
		{
			Json::Value resJson;
			resJson["code"] = k400BadRequest;
			resJson["msg"] = "No json object is found in the request";
			auto resp = HttpResponse::newHttpJsonResponse(resJson);
			resp->setStatusCode(k400BadRequest);
			return callback(resp);
		}

		(*jsonPtr)["id"] = id;

		Users object;
		try
		{
			std::string err;
			if (!Users::validateJsonForUpdate(*jsonPtr, err))
			{
				Json::Value ret;
				ret["code"] = k400BadRequest;
				ret["msg"] = err;
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				resp->setStatusCode(k400BadRequest);
				return callback(resp);
			}
			object.updateByJson(*jsonPtr);
		}
		catch (const std::exception &e)
		{
			LOG_ERROR << e.what();
			Json::Value ret;
			ret["code"] = k400BadRequest;
			ret["msg"] = "Field type error";
			auto resp = HttpResponse::newHttpJsonResponse(ret);
			resp->setStatusCode(k400BadRequest);
			return callback(resp);
		}

		auto dbClientPtr = getDbClient();
		auto callbackPtr =
				std::make_shared<std::function<void(const HttpResponsePtr &)>>(
						std::move(callback));
		drogon::orm::Mapper<Users> mapper(dbClientPtr);

		mapper.update(
				object,
				[callbackPtr](const size_t count)
				{
					if (count == 1)
					{
						Json::Value resJson;
						resJson["code"] = k200OK;
						resJson["msg"] = "successed";
						auto resp = HttpResponse::newHttpJsonResponse(resJson);
						return (*callbackPtr)(resp);
					}
					else if (count == 0)
					{
						Json::Value ret;
						ret["code"] = k404NotFound;
						ret["msg"] = "No resources are updated";
						auto resp = HttpResponse::newHttpJsonResponse(ret);
						resp->setStatusCode(k404NotFound);
						return (*callbackPtr)(resp);
					}
					else
					{
						LOG_FATAL << "More than one resource is updated: " << count;
						Json::Value ret;
						ret["code"] = k500InternalServerError;
						ret["msg"] = "database error";
						auto resp = HttpResponse::newHttpJsonResponse(ret);
						resp->setStatusCode(k500InternalServerError);
						return (*callbackPtr)(resp);
					}
				},
				[callbackPtr](const DrogonDbException &e)
				{
					LOG_ERROR << e.base().what();
					Json::Value ret;
					ret["code"] = k500InternalServerError;
					ret["msg"] = "database error";
					auto resp = HttpResponse::newHttpJsonResponse(ret);
					resp->setStatusCode(k500InternalServerError);
					return (*callbackPtr)(resp);
				});
	}

	void User::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, Users::PrimaryKeyType &&id)
	{
		auto dbClientPtr = getDbClient();
		auto callbackPtr =
				std::make_shared<std::function<void(const HttpResponsePtr &)>>(
						std::move(callback));
		orm::Mapper<Users> mapper(dbClientPtr);

		mapper.findByPrimaryKey(
				id,
				[req, callbackPtr, this](Users user)
				{
					Json::Value resJson;
					resJson["code"] = k200OK;
					resJson["data"] = user.toJson();
					return (*callbackPtr)(HttpResponse::newHttpJsonResponse(resJson));
				},
				[callbackPtr](const DrogonDbException &e)
				{
					const orm::UnexpectedRows *s = dynamic_cast<const orm::UnexpectedRows *>(&e.base());
					if (s)
					{
						auto resp = HttpResponse::newHttpResponse();
						resp->setStatusCode(k404NotFound);
						return (*callbackPtr)(resp);
					}
					LOG_ERROR << e.base().what();
					Json::Value ret;
					ret["msg"] = "database error";
					auto resp = HttpResponse::newHttpJsonResponse(ret);
					resp->setStatusCode(k500InternalServerError);
					return (*callbackPtr)(resp);
				});
	}

}