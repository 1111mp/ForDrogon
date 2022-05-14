/**
 *
 *  User.cc
 *  This file is generated by drogon_ctl
 *
 */

#include "User.h"

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
						resultJson["msg"] = "Incorrect password.";
						return (*callbackPtr)(HttpResponse::newHttpJsonResponse(resultJson));
					}

					try
					{
						auto userJson = makeJson(req, user);
						userJson.removeMember("pwd");

						Utils::jwt::JWT jwtGenerated = Utils::jwt::JWT::generateToken(
								{
										{"userid", picojson::value(userJson["id"].asInt64())},
								},
								jsonPtr->isMember("member") && (*jsonPtr)["member"].asBool());

						auto token = jwtGenerated.getToken();
						auto uuid = Utils::uuid::genUUID();

						auto redisClientPtr = getRedisClient();

						redisClientPtr->newTransactionAsync(
								[uuid, token, userJson, callbackPtr](const nosql::RedisTransactionPtr &transPtr)
								{
									auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + userJson["id"].asString();

									LOG_INFO << auth;
									LOG_INFO << uuid;

									transPtr->execCommandAsync(
											[](const drogon::nosql::RedisResult &r) { /* this command works */ },
											[](const std::exception &err) { /* this command failed */ },
											"del %s", auth.data());

									transPtr->execCommandAsync(
											[](const drogon::nosql::RedisResult &r) { /* this command works */ },
											[](const std::exception &err) { /* this command failed */ },
											"hset %s %s %s", auth.data(), uuid.data(), token.data());

									transPtr->execute(
											[uuid, userJson, callbackPtr](const drogon::nosql::RedisResult &r)
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
					LOG_ERROR << e.base().what();
					Json::Value resultJson;
					resultJson["code"] = k500InternalServerError;
					resultJson["msg"] = "database error";
					return (*callbackPtr)(HttpResponse::newHttpJsonResponse(resultJson));
				});
	}

}