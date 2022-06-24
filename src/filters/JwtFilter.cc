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
		// Skip the verification on method Options
		if (req->getMethod() == HttpMethod::Options)
			return fccb();

		auto headers = req->getHeaders();
		auto token = headers["token"], userid = headers["userid"];

		if (token.empty() || userid.empty())
		{
			Json::Value resultJson;
			resultJson["code"] = k401Unauthorized;
			resultJson["msg"] = "Authentication Error.";

			auto res = HttpResponse::newHttpJsonResponse(resultJson);
			res->setStatusCode(k401Unauthorized);

			return fcb(res);
		}

		try
		{
			auto redisClientPtr = app().getRedisClient();
			auto auth = app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + userid;
			auto bcryptStr = redisClientPtr->execCommandSync<std::string>(
					[](const nosql::RedisResult &r)
					{
						if (r.type() == nosql::RedisResultType::kNil)
							return std::string{};
						return r.asString();
					},
					"hget %s %s", auth.data(), token.data());

			if (bcryptStr.empty())
			{
				Json::Value resultJson;
				resultJson["code"] = k401Unauthorized;
				resultJson["msg"] = "Token is invalid.";
				auto res = HttpResponse::newHttpJsonResponse(resultJson);
				res->setStatusCode(k401Unauthorized);
				return fcb(res);
			}

			std::map<std::string, drogon::any> jwtAttributes = JWT::decodeToken(bcryptStr);

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