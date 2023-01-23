#pragma once

#include <string>
#include <json/json.h>

namespace Utils
{
  class JsonUtil
  {
  public:
    static Json::Value parse(const std::string &);
    static const std::string stringify(const Json::Value &);
  };

} // namespace Utils::Json
