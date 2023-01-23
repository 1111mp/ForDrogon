#include "JwtFilterForWS.h"
#include "jwt/JWT.h"
#include "common/Common.h"

using namespace Utils::jwt;

namespace api::v1::filters
{
  Task<HttpResponsePtr> JwtFilterForWS::doFilter(const HttpRequestPtr &req)
  {
    // Skip the verification on method Options
    if (req->getMethod() == HttpMethod::Options)
      co_return {};

    auto token = req->getParameter("token"),
         userid = req->getParameter("userid");
    if (token.empty() || userid.empty())
    {
      auto resp = Utils::Common::makeHttpJsonResponse(k401Unauthorized, "Authentication Error.");
      co_return resp;
    }

    try
    {
      auto redisClientPtr = getRedisClient();
      auto auth = drogon::app().getCustomConfig()["redis"]["auth_key"].asString() + "_" + userid;

      auto result = co_await redisClientPtr->execCommandCoro("hget %s %s", auth.data(), token.data());
      if (result.isNil())
      {
        auto resp = Utils::Common::makeHttpJsonResponse(k401Unauthorized, "Authentication is invalid.");
        co_return resp;
      }

      auto jwtAttributes = JWT::decodeToken(result.asString());

      if ((&jwtAttributes) == nullptr)
      {
        auto resp = Utils::Common::makeHttpJsonResponse(k401Unauthorized, "Authentication is invalid.");
        co_return resp;
      }

      auto member = jwtAttributes.member;
      if (!member)
      {
        co_await redisClientPtr->execCommandCoro("expire %s %i", auth.data(), 3600);
      }

      // Save the claims on attributes, for on next endpoint to be accessible
      req->getAttributes()->insert("jwt_userid", jwtAttributes.userid);

      co_return {};
    }
    catch (const std::exception &err)
    {
      LOG_ERROR << "JwtFilter error: " << err.what();
      auto resp = Utils::Common::makeHttpJsonResponse(k401Unauthorized, err.what());
      co_return resp;
    }
  }

} // namespace api::v1::filters
