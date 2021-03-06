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

	Task<void> User::login(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
	{
		auto jsonPtr = req->getJsonObject();

		if (!jsonPtr || !jsonPtr->isMember("account") || !jsonPtr->isMember("pwd"))
		{
			Json::Value resultJson;
			resultJson["code"] = k400BadRequest;
			resultJson["msg"] = "Missing account or password.";
			auto resp = HttpResponse::newHttpJsonResponse(resultJson);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			co_return;
		}

		auto dbClientPtr = getDbClient();
		CoroMapper<Users> mapper(dbClientPtr);

		try
		{
			auto user = co_await mapper.findOne(Criteria(Users::Cols::_account, CompareOperator::EQ, (*jsonPtr)["account"]));

			if (!Utils::Bcrypt::compare((*jsonPtr)["pwd"].asString(), user.getPwd()->data()))
			{
				// Compare password hash.
				Json::Value resultJson;
				resultJson["code"] = k403Forbidden;
				resultJson["msg"] = "Incorrect account or password.";
				callback(HttpResponse::newHttpJsonResponse(resultJson));
				co_return;
			}

			auto userJson = user.toJson();
			userJson.removeMember("pwd");
			auto member = jsonPtr->isMember("member") && (*jsonPtr)["member"].asBool();
			auto userid = userJson["id"].asInt64();

			try
			{
				Utils::jwt::JWT jwtGenerated = Utils::jwt::JWT::generateToken(
						{
								{"userid", picojson::value(userJson["id"].asInt64())},
								{"member", picojson::value(member)},
						},
						member);

				auto token = jwtGenerated.getToken();
				auto uuid = Utils::uuid::genUUID();
				auto expiresAt = member ? jwtGenerated.getExpiration() : 3600;
				auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + userJson["id"].asString();
				auto redisClientPtr = getRedisClient();

				auto transPtr = co_await redisClientPtr->newTransactionCoro();
				co_await transPtr->execCommandCoro("del %s", auth.data());
				co_await transPtr->execCommandCoro("hset %s %s %s", auth.data(), uuid.data(), token.data());
				co_await transPtr->execCommandCoro("expire %s %lld", auth.data(), expiresAt);
				co_await transPtr->executeCoro();

				Json::Value resultJson;
				resultJson["code"] = k200OK;
				resultJson["token"] = uuid;
				resultJson["data"] = userJson;
				callback(HttpResponse::newHttpJsonResponse(resultJson));
				co_return;
			}
			catch (const std::exception &err)
			{
				LOG_ERROR << err.what();
				Json::Value ret;
				ret["code"] = k500InternalServerError;
				ret["msg"] = err.what();
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
				co_return;
			}
		}
		catch (const DrogonDbException &err)
		{
			const orm::UnexpectedRows *s = dynamic_cast<const orm::UnexpectedRows *>(&err.base());
			if (s)
			{
				auto resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(k404NotFound);
				callback(resp);
				co_return;
			}
			LOG_ERROR << err.base().what();
			Json::Value resultJson;
			resultJson["code"] = k500InternalServerError;
			resultJson["msg"] = "database error.";
			auto resp = HttpResponse::newHttpJsonResponse(resultJson);
			resp->setStatusCode(k500InternalServerError);
			callback(resp);
			co_return;
		}
	}

	Task<void> User::logout(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
	{
		auto userID = req->getAttributes()->get<int64_t>("jwt_userid");
		auto redisClientPtr = getRedisClient();
		auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + std::to_string(userID);

		try
		{
			redisClientPtr->execCommandCoro("del %s", auth.data());

			Json::Value resultJson;
			resultJson["code"] = k200OK;
			resultJson["msg"] = "Sign out successfully.";
			callback(HttpResponse::newHttpJsonResponse(resultJson));
		}
		catch (const std::exception &err)
		{
			LOG_ERROR << err.what();
			Json::Value resultJson;
			resultJson["code"] = k500InternalServerError;
			resultJson["msg"] = err.what();
			auto resp = HttpResponse::newHttpJsonResponse(resultJson);
			resp->setStatusCode(k500InternalServerError);
			callback(resp);
		}
		co_return;
	}

	Task<void> User::create(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
	{
		auto jsonPtr = req->getJsonObject();
		if (!jsonPtr || !jsonPtr->isMember("account") || !jsonPtr->isMember("pwd"))
		{
			Json::Value resultJson;
			resultJson["code"] = k400BadRequest;
			resultJson["msg"] = "Missing account or password.";
			auto resp = HttpResponse::newHttpJsonResponse(resultJson);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			co_return;
		}

		Users object = Users(*jsonPtr);
		auto now = trantor::Date::now();
		object.setRegistime(now);
		object.setUpdatetime(now);

		auto dbClientPtr = getDbClient();
		CoroMapper<Users> mapper(dbClientPtr);

		try
		{
			auto user = co_await mapper.insert(object);
			auto userJson = user.toJson();
			userJson.removeMember("pwd");
			auto member = jsonPtr->isMember("member") && (*jsonPtr)["member"].asBool();

			try
			{
				Utils::jwt::JWT jwtGenerated = Utils::jwt::JWT::generateToken(
						{
								{"userid", picojson::value(userJson["id"].asInt64())},
								{"member", picojson::value(member)},
						},
						member);

				auto token = jwtGenerated.getToken();
				auto uuid = Utils::uuid::genUUID();
				auto expiresAt = member ? jwtGenerated.getExpiration() : 3600;
				auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + userJson["id"].asString();

				auto redisClientPtr = getRedisClient();
				auto transPtr = co_await redisClientPtr->newTransactionCoro();
				co_await transPtr->execCommandCoro("hset %s %s %s", auth.data(), uuid.data(), token.data());
				co_await transPtr->execCommandCoro("expire %s %lld", auth.data(), expiresAt);
				co_await transPtr->executeCoro();

				Json::Value resultJson;
				resultJson["code"] = k200OK;
				resultJson["token"] = uuid;
				resultJson["data"] = userJson;
				callback(HttpResponse::newHttpJsonResponse(resultJson));
				co_return;
			}
			catch (const std::exception &err)
			{
				LOG_ERROR << err.what();
				Json::Value resultJson;
				resultJson["code"] = k500InternalServerError;
				resultJson["msg"] = err.what();
				auto resp = HttpResponse::newHttpJsonResponse(resultJson);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
				co_return;
			}
		}
		catch (const DrogonDbException &err)
		{
			const orm::UniqueViolation *s = dynamic_cast<const orm::UniqueViolation *>(&err.base());
			Json::Value resultJson;
			if (s)
			{
				resultJson["code"] = k409Conflict;
				resultJson["msg"] = "The account already exists.";
				auto resp = HttpResponse::newHttpJsonResponse(resultJson);
				resp->setStatusCode(k409Conflict);
				callback(resp);
				co_return;
			}

			LOG_ERROR << err.base().what();
			resultJson["code"] = k500InternalServerError;
			resultJson["msg"] = "database error";
			auto resp = HttpResponse::newHttpJsonResponse(resultJson);
			resp->setStatusCode(k500InternalServerError);
			callback(resp);
			co_return;
		}
	}

	Task<void> User::deleteOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
	{
		auto userID = req->getAttributes()->get<int64_t>("jwt_userid");
		auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + std::to_string(userID);

		auto dbClientPtr = getDbClient();
		CoroMapper<Users> mapper(dbClientPtr);

		Json::Value ret;
		try
		{
			auto count = co_await mapper.deleteByPrimaryKey(userID);

			if (count == 1)
			{
				auto redisClientPtr = getRedisClient();
				co_await redisClientPtr->execCommandCoro("del %s", auth.data());
				ret["code"] = k200OK;
				ret["msg"] = "Successfully.";
				callback(HttpResponse::newHttpJsonResponse(ret));
			}
			else if (count == 0)
			{
				ret["code"] = k404NotFound;
				ret["msg"] = "No resources deleted";
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				resp->setStatusCode(k404NotFound);
				callback(resp);
			}
			else
			{
				LOG_FATAL << "Delete more than one records: " << count;
				ret["code"] = k500InternalServerError;
				ret["msg"] = "Something error.";
				auto resp = HttpResponse::newHttpJsonResponse(ret);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			}
		}
		catch (const DrogonDbException &err)
		{
			ret["code"] = k500InternalServerError;
			ret["msg"] = "Database error.";
			auto resp = HttpResponse::newHttpJsonResponse(ret);
			resp->setStatusCode(k500InternalServerError);
			callback(resp);
		}
		co_return;
	}

	Task<void> User::updateOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
	{
		auto jsonPtr = req->jsonObject();
		auto id = req->getAttributes()->get<int64_t>("jwt_userid");
		Json::Value resJson;
		if (!jsonPtr)
		{
			resJson["code"] = k400BadRequest;
			resJson["msg"] = "No json object is found in the request";
			auto resp = HttpResponse::newHttpJsonResponse(resJson);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			co_return;
		}

		(*jsonPtr)["id"] = id;

		Users object;
		object.setUpdatetime(trantor::Date::now());
		try
		{
			std::string err;
			if (!Users::validateJsonForUpdate(*jsonPtr, err))
			{
				resJson["code"] = k400BadRequest;
				resJson["msg"] = err;
				auto resp = HttpResponse::newHttpJsonResponse(resJson);
				resp->setStatusCode(k400BadRequest);
				callback(resp);
				co_return;
			}
			object.updateByJson(*jsonPtr);
		}
		catch (const std::exception &e)
		{
			LOG_ERROR << e.what();
			resJson["code"] = k400BadRequest;
			resJson["msg"] = "Field type error";
			auto resp = HttpResponse::newHttpJsonResponse(resJson);
			resp->setStatusCode(k400BadRequest);
			callback(resp);
			co_return;
		}

		auto dbClientPtr = getDbClient();
		CoroMapper<Users> mapper(dbClientPtr);

		try
		{
			auto count = co_await mapper.update(object);
			if (count == 1)
			{
				resJson["code"] = k200OK;
				resJson["msg"] = "successed";
				auto resp = HttpResponse::newHttpJsonResponse(resJson);
				callback(resp);
			}
			else if (count == 0)
			{
				resJson["code"] = k404NotFound;
				resJson["msg"] = "No resources are updated";
				auto resp = HttpResponse::newHttpJsonResponse(resJson);
				resp->setStatusCode(k404NotFound);
				callback(resp);
			}
			else
			{
				LOG_FATAL << "More than one resource is updated: " << count;
				resJson["code"] = k500InternalServerError;
				resJson["msg"] = "database error";
				auto resp = HttpResponse::newHttpJsonResponse(resJson);
				resp->setStatusCode(k500InternalServerError);
				callback(resp);
			}
		}
		catch (const DrogonDbException &err)
		{
			LOG_ERROR << err.base().what();
			resJson["code"] = k500InternalServerError;
			resJson["msg"] = "database error.";
			auto resp = HttpResponse::newHttpJsonResponse(resJson);
			resp->setStatusCode(k500InternalServerError);
			callback(resp);
		}
		co_return;
	}

	Task<void> User::getOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, Users::PrimaryKeyType id)
	{
		auto dbClientPtr = getDbClient();
		CoroMapper<Users> mapper(dbClientPtr);

		Json::Value resJson;
		try
		{
			auto userJson = (co_await mapper.findByPrimaryKey(id)).toJson();
			userJson.removeMember("pwd");

			resJson["code"] = k200OK;
			resJson["data"] = userJson;
			callback(HttpResponse::newHttpJsonResponse(resJson));
		}
		catch (const DrogonDbException &err)
		{
			const orm::UnexpectedRows *s = dynamic_cast<const orm::UnexpectedRows *>(&err.base());
			if (s)
			{
				resJson["code"] = k404NotFound;
				resJson["msg"] = "User[" + std::to_string(id) + "] not found.";
				auto resp = HttpResponse::newHttpJsonResponse(resJson);
				resp->setStatusCode(k404NotFound);
				callback(resp);
				co_return;
			}
			LOG_ERROR << err.base().what();
			resJson["code"] = k500InternalServerError;
			resJson["msg"] = "database error";
			auto resp = HttpResponse::newHttpJsonResponse(resJson);
			resp->setStatusCode(k500InternalServerError);
			callback(resp);
		}
		co_return;
	}

}