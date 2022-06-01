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
						if (r.type() == nosql::RedisResultType::kNil)
							return std::string{};
						return r.asString();
					},
					"hget %s %s", auth.data(), headers["token"].data());

			if (token.empty())
			{
				Json::Value resultJson;
				resultJson["code"] = k401Unauthorized;
				resultJson["msg"] = "Token is invalid.";
				auto res = HttpResponse::newHttpJsonResponse(resultJson);
				res->setStatusCode(k401Unauthorized);
				return fcb(res);
			}

			std::map<std::string, drogon::any> jwtAttributes = JWT::decodeToken(token);

			if (jwtAttributes.empty())
			{
				Json::Value resultJson;
				resultJson["code"] = k401Unauthorized;
				resultJson["msg"] = "Token is invalid.";
				auto res = HttpResponse::newHttpJsonResponse(resultJson);
				res->setStatusCode(k401Unauthorized);
				return fcb(res);
			}

			auto member = std::any_cast<bool>(jwtAttributes["member"]);
			auto userid = std::any_cast<int64_t>(jwtAttributes["userid"]);
			if (!member)
			{
				// The validity period is automatically extended by one hour.
				redisClientPtr->execCommandSync<int>(
						[](const nosql::RedisResult &r)
						{
							return r.asInteger();
						},
						"expire %s %i", auth.data(), 3600);
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