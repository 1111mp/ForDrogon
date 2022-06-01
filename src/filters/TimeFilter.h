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

  class TimeFilter : public HttpFilter<TimeFilter>
  {
  public:
    TimeFilter() = default;
    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;

  private:
    void updateCache(const std::string &key, const trantor::Date &date);

    nosql::RedisClientPtr getRedisClient()
    {
      return drogon::app().getRedisClient(redieClientName_);
    }

  private:
    const std::string redieClientName_{"default"};
  };

}