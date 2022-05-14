/**
 *
 *  JwtFilter.cc
 *
 */

#include "JwtFilter.h"
#include "jwt/JWT.h"

using namespace drogon;
using namespace Utils::jwt;

namespace api::v1::filters
{

	void JwtFilter::doFilter(const HttpRequestPtr &req,
													 FilterCallback &&fcb,
													 FilterChainCallback &&fccb)
	{
		auto headers = req->getHeaders();

		if (headers["token"].empty() || headers["userid"].empty())
		{
			Json::Value resultJson;
			resultJson["code"] = k401Unauthorized;
			resultJson["msg"] = "Authentication Error";

			auto res = HttpResponse::newHttpJsonResponse(resultJson);
			res->setStatusCode(k401Unauthorized);

			return fcb(res);
		}

		try
		{
			auto redisClientPtr = app().getRedisClient();
			auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + headers["userid"];
			auto token = redisClientPtr->execCommandSync<std::string>(
					[](const nosql::RedisResult &r)
					{
						return r.asString();
					},
					"hget %s %s", auth.data(), headers["token"].data());

			std::map<std::string, drogon::any> jwtAttributes = JWT::decodeToken(token);

			if (jwtAttributes.empty())
			{
				Json::Value resultJson;
				resultJson["code"] = k401Unauthorized;
				resultJson["msg"] = "Token is invalid!";
				auto res = HttpResponse::newHttpJsonResponse(resultJson);
				res->setStatusCode(k401Unauthorized);
				return fcb(res);
			}

			// Save the claims on attributes, for on next endpoint to be accessible
			for (auto &attribute : jwtAttributes)
				req->getAttributes()->insert("jwt_" + attribute.first, attribute.second);

			return fccb();
		}
		catch (const std::exception &err)
		{
			LOG_ERROR << "JwtFilter error: " << err.what();
			Json::Value resultJson;
			resultJson["code"] = k401Unauthorized;
			resultJson["msg"] = err.what();
			auto res = HttpResponse::newHttpJsonResponse(resultJson);
			res->setStatusCode(k401Unauthorized);
			return fcb(res);
		}
	}

}