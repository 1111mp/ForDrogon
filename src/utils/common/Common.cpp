#include "Common.h"

namespace Utils::Common
{
  drogon::HttpResponsePtr makeHttpJsonResponse(
      const drogon::HttpStatusCode &code,
      const std::string &msg,
      const Json::Value &data)
  {
    Json::Value resultJson;
    resultJson["code"] = code;
    resultJson["msg"] = msg;
    if (!data.empty())
    {
      resultJson["data"] = data;
    }
    auto resp = drogon::HttpResponse::newHttpJsonResponse(resultJson);
    return resp;
  }
}