/**
 *
 *  TimeFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/nosql/RedisClient.h>

using namespace drogon;

namespace api::v1::filters
{

  class TimeFilter : public HttpCoroFilter<TimeFilter>
  {
  public:
    TimeFilter() = default;
    virtual Task<HttpResponsePtr> doFilter(const HttpRequestPtr &req) override;

  private:
    Task<void> updateCache(const std::string &key, const trantor::Date &date);

    nosql::RedisClientPtr getRedisClient()
    {
      return drogon::app().getFastRedisClient(redieClientName_);
    }

  private:
    const std::string redieClientName_{"default"};
  };

}