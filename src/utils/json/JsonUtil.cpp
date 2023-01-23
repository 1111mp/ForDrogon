#include "JsonUtil.h"

namespace Utils
{
  Json::Value JsonUtil::parse(const std::string &rawJson)
  {
    Json::CharReaderBuilder builder;
    Json::Value root;
    JSONCPP_STRING err;

    const auto rawJsonLength = static_cast<int>(rawJson.length());
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root, &err))
    {
      throw err;
    }

    return root;
  }

  const std::string JsonUtil::stringify(const Json::Value &root)
  {
    Json::StreamWriterBuilder builder;
    const std::string json_file = Json::writeString(builder, root);
    return json_file;
  }
} // namespace Utils
