#pragma once

#include <drogon/HttpController.h>

#include "ChatGroups.h"
#include "Users.h"
#include "GroupMembers.h"

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::database_test;

namespace api::v1
{

  class Group : public HttpController<Group>
  {
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(Group::create, "", Post, Options, "api::v1::filters::JwtFilter");
    METHOD_ADD(Group::deleteOne, "", Delete, Options, "api::v1::filters::JwtFilter");
    METHOD_ADD(Group::updateOne, "", Put, Options, "api::v1::filters::JwtFilter");
    METHOD_ADD(Group::get, "", Get, Options, "api::v1::filters::JwtFilter");
    METHOD_LIST_END

  public:
    void create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void deleteOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

  private:
    orm::DbClientPtr getDbClient()
    {
      return drogon::app().getDbClient(dbClientName_);
    }

    nosql::RedisClientPtr getRedisClient()
    {
      return drogon::app().getRedisClient(redieClientName_);
    }

  private:
    const std::string dbClientName_{"default"};
    const std::string redieClientName_{"default"};
  };

}