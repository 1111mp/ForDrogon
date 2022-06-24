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
    METHOD_ADD(Group::deleteOne, "/{1}", Delete, Options, "api::v1::filters::JwtFilter");
    METHOD_ADD(Group::updateOne, "", Put, Options, "api::v1::filters::JwtFilter");
    METHOD_ADD(Group::getOne, "/{1}", Get, Options, "api::v1::filters::JwtFilter");
    METHOD_LIST_END

  public:
    Task<void> create(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback);
    Task<void> deleteOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, ChatGroups::PrimaryKeyType id);
    Task<void> updateOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback);
    Task<void> getOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, ChatGroups::PrimaryKeyType &&id);

  private:
    orm::DbClientPtr getDbClient()
    {
      return drogon::app().getDbClient(dbClientName_);
    }

    nosql::RedisClientPtr getRedisClient()
    {
      return drogon::app().getRedisClient(redieClientName_);
    }

    Task<Users> getUser(const DbClientPtr &clientPtr, const int32_t &id)
    {
      auto result = co_await clientPtr->execSqlCoro("select * from users where id = ?", id);

      co_return Users(result[0]);
    }

    Task<std::vector<GroupMembers>> getMembers(const DbClientPtr &clientPtr, const int32_t &id)
    {
      auto members = co_await clientPtr->execSqlCoro("select * from group_members where groupId = ?", id);

      std::vector<GroupMembers> ret;
      ret.reserve(members.size());
      for (auto const &member : members)
      {
        ret.emplace_back(GroupMembers(member));
      }
      co_return ret;
    }

    Task<std::vector<Users>> getMembersWithInfo(const DbClientPtr &clientPtr, const int32_t &id)
    {
      auto members = co_await clientPtr->execSqlCoro("select * from group_members where groupId = ?", id);

      std::vector<Users> ret;
      ret.reserve(members.size());
      for (auto const &member : members)
      {
        auto user = co_await clientPtr->execSqlCoro("select * from users where id = ?", member["userId"].as<int32_t>());
        ret.emplace_back(Users(user[0]));
      }
      co_return ret;
    }

  private:
    const std::string dbClientName_{"default"};
    const std::string redieClientName_{"default"};
  };

}