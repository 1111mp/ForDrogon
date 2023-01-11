#pragma once

#include <drogon/drogon.h>
#include <jwt-cpp/jwt.h>

namespace Utils::jwt
{

  struct InfoForUser
  {
    bool member;
    int64_t userid;
  };

  class JWT
  {
  public:
    JWT(const std::string &token, const int64_t &expiration)
        : token(token), expiration(expiration) {}

    std::string getToken() const
    {
      return token;
    }

    int64_t getExpiration() const
    {
      return expiration;
    }

    static JWT generateToken(const std::map<std::string, ::jwt::traits::kazuho_picojson::value_type> &claims = {}, const bool &extension = false);
    static InfoForUser decodeToken(const std::string &encodedToken);

  private:
    std::string token;
    int64_t expiration;

    static bool verifyToken(const ::jwt::decoded_jwt<::jwt::traits::kazuho_picojson> &jwt);
  };

}