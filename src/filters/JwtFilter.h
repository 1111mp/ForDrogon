/**
 *
 *  JwtFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/nosql/RedisClient.h>

using namespace drogon;

namespace api::v1::filters
{

  class JwtFilter : public drogon::HttpCoroFilter<JwtFilter>
  {
  public:
    JwtFilter() = default;

    virtual Task<HttpResponsePtr> doFilter(const HttpRequestPtr &req) override;

  private:
    nosql::RedisClientPtr getRedisClient()
    {
      return drogon::app().getFastRedisClient(redieClientName_);
    }

  private:
    const std::string redieClientName_{"default"};
  };

}
