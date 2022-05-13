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

  class JwtFilter : public HttpFilter<JwtFilter>
  {
  public:
    JwtFilter() = default;

    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;
  };

}
