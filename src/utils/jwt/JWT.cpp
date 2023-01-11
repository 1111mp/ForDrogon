#include "JWT.h"

#include <chrono>

namespace Utils::jwt
{

  JWT JWT::generateToken(const std::map<std::string, ::jwt::traits::kazuho_picojson::value_type> &claims, const bool &extension)
  {
    const auto time = std::chrono::system_clock::now();

    // If remember is true, just add more 30 days, otherwise, just put valid only for 1 day.
    const int64_t expiresAt = std::chrono::duration_cast<std::chrono::seconds>((time + std::chrono::hours{(extension ? 30 : 1) * 24}).time_since_epoch()).count();

    auto jwtToken = ::jwt::create()
                        .set_type("JWT")
                        .set_issuer(drogon::app().getCustomConfig()["jwt"]["issuer"].asString())
                        .set_audience(drogon::app().getCustomConfig()["jwt"]["audience"].asString())
                        .set_issued_at(time)
                        .set_not_before(time)
                        .set_expires_at(std::chrono::system_clock::from_time_t(expiresAt));

    for (auto &claim : claims)
      jwtToken.set_payload_claim(claim.first, claim.second);

    return {
        jwtToken.sign(::jwt::algorithm::hs256{drogon::app().getCustomConfig()["jwt"]["private_key"].asString()}),
        expiresAt,
    };
  }

  InfoForUser JWT::decodeToken(const std::string &encodedToken)
  {
    // Let's decode it, if isn't a valid token of JWT, catch will be called
    try
    {
      auto decodedToken = ::jwt::decode(encodedToken);

      if (verifyToken(decodedToken))
      {
        auto member = decodedToken.get_payload_claim("member").as_boolean();
        auto userid = decodedToken.get_payload_claim("userid").as_integer();
        return {member, userid};
      }

      throw;
    }
    catch (const std::exception &e)
    {
      return {};
    }
  }

  bool JWT::verifyToken(const ::jwt::decoded_jwt<::jwt::traits::kazuho_picojson> &jwt)
  {
    // Let's create a verifier
    auto jwtVerifier = ::jwt::verify()
                           .with_issuer(drogon::app().getCustomConfig()["jwt"]["issuer"].asString())
                           .with_audience(drogon::app().getCustomConfig()["jwt"]["audience"].asString())
                           .allow_algorithm(::jwt::algorithm::hs256{drogon::app().getCustomConfig()["jwt"]["private_key"].asString()});

    // If some properties of token doesn't correspond same as like, issued, audience, etc..., catch will be called
    try
    {
      jwtVerifier.verify(jwt);
      return true;
    }
    catch (const std::exception &e)
    {
      return false;
    }
  }

} // namespace Utils::jwt
