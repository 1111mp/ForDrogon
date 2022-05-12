/**
 *
 *  LoginFilter.cc
 *
 */

#include "LoginFilter.h"
#include "jwt/JWT.h"

using namespace drogon;
using namespace Utils::jwt;

void LoginFilter::doFilter(const HttpRequestPtr &req,
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

	auto redisClient = app().getRedisClient();

	redisClient->execCommandAsync(
			[req, fcb = std::move(fcb), fccb = std::move(fccb)](const nosql::RedisResult &result)
			{
				if (result.type() == nosql::RedisResultType::kNil)
				{
					Json::Value resultJson;
					resultJson["code"] = k401Unauthorized;
					resultJson["msg"] = "Authentication Error";

					auto res = HttpResponse::newHttpJsonResponse(resultJson);
					res->setStatusCode(k401Unauthorized);

					return fcb(res);
				}

				std::map<std::string, drogon::any> jwtAttributes = JWT::decodeToken(result.asString());

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
			},
			[fcb](const nosql::RedisException &err)
			{
				LOG_ERROR << "LoginFilter error: " << err.what();

				Json::Value resultJson;
				resultJson["code"] = k401Unauthorized;
				resultJson["msg"] = err.what();

				auto res = HttpResponse::newHttpJsonResponse(resultJson);
				res->setStatusCode(k401Unauthorized);

				return fcb(res);
			},
			"get %s", headers["token"].data());
}
