#pragma once

#include <drogon/HttpController.h>

namespace Utils::Common
{
  drogon::HttpResponsePtr makeHttpJsonResponse(
      const drogon::HttpStatusCode &code,
      const std::string &msg,
      const Json::Value &data = Json::objectValue);
}