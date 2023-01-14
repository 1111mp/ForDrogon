/**
 *
 *  JwtFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;

namespace api::v1::filters
{

  class JwtFilter : public drogon::HttpCoroFilter<JwtFilter>
  {
  public:
    JwtFilter() = default;

    virtual Task<HttpResponsePtr> doFilter(const HttpRequestPtr &req) override;
  };

}
