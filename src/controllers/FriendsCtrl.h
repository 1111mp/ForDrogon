#pragma once

#include <drogon/HttpController.h>

using namespace drogon;
using namespace drogon::orm;

namespace api::v1
{
  class Friends : public drogon::HttpController<Friends>
  {
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(Friends::create, "/add", Post, Options, "api::v1::filters::JwtFilter");
    METHOD_LIST_END

  public:
    Task<void> create(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback);

  private:
    orm::DbClientPtr getDbClient()
    {
      return drogon::app().getFastDbClient(dbClientName_);
    }

    nosql::RedisClientPtr getRedisClient()
    {
      return drogon::app().getFastRedisClient(redieClientName_);
    }

  private:
    const std::string dbClientName_{"default"};
    const std::string redieClientName_{"default"};
  };

} // namespace api::v1
