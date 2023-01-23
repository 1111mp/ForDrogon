#include "IMGroupCtrl.h"
#include "common/Common.h"

namespace api::v1
{

  Task<void> Group::create(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
  {
    auto jsonPtr = *(req->getJsonObject());
    auto type = jsonPtr["type"].asInt();
    auto members = jsonPtr["members"];

    if ((type != 1 && type != 2) || !members.isArray() || members.size() == 0)
    {
      auto resp = Utils::Common::makeHttpJsonResponse(k400BadRequest, "Invalid parameter.");
      callback(resp);
      co_return;
    }

    auto dbClientPtr = getDbClient();
    auto userID = req->getAttributes()->get<int64_t>("jwt_userid");

    try
    {
      auto transPtr = co_await dbClientPtr->newTransactionCoro();

      CoroMapper<ChatGroups> groupMapper(transPtr);
      auto groupObject = ChatGroups(jsonPtr);
      groupObject.setCreator(userID);
      groupObject.setMax(type == 1 ? 200 : 2000);

      auto groupJson = (co_await groupMapper.insert(groupObject)).toJson();

      CoroMapper<GroupMembers> memberMapper(transPtr);
      for (unsigned int index = 0; index < members.size(); ++index)
      {
        Json::Value object;
        object["groupId"] = groupJson["id"].asInt64();
        object["userId"] = members[index].asInt64();

        try
        {
          auto member = co_await memberMapper.insert(GroupMembers(object));

          auto id = member.getUserid().get();
          auto userJson = (co_await getUser(transPtr, *id)).toJson();
          userJson.removeMember("pwd");
          groupJson["members"].append(userJson);
        }
        catch (const DrogonDbException &err)
        {
          const orm::SqlError *s = dynamic_cast<const orm::SqlError *>(&err.base());
          if (s)
          {
            auto resp = Utils::Common::makeHttpJsonResponse(k404NotFound, "Invalid userid: " + members[index].asString());
            callback(resp);
            co_return;
          }
          LOG_ERROR << err.base().what();
          auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "database error.");
          callback(resp);
          co_return;
        }
      }

      auto resp = Utils::Common::makeHttpJsonResponse(k200OK, "created successfully.", groupJson);
      callback(resp);
    }
    catch (const Failure &err)
    {
      LOG_ERROR << err.what();
      auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "database error.");
      callback(resp);
    }

    co_return;
  }

  Task<void> Group::deleteOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, ChatGroups::PrimaryKeyType &&id)
  {
    auto dbClientPtr = getDbClient();
    try
    {
      auto transPtr = co_await dbClientPtr->newTransactionCoro();
      CoroMapper<ChatGroups> groupMapper(transPtr);
      auto members = co_await getMembers(transPtr, id);
      CoroMapper<GroupMembers> memberMapper(transPtr);
      for (auto const &member : members)
      {
        auto count = co_await memberMapper.deleteByPrimaryKey(member.getPrimaryKey());
        if (count != 1)
        {
          transPtr->rollback();
          auto userId = member.getUserid();
          auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "Error deleting user: " + *userId);
          callback(resp);
          co_return;
        }
      }
      auto count = co_await groupMapper.deleteByPrimaryKey(id);
      if (count != 1)
      {
        transPtr->rollback();
        auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "Error deleting group: " + id);
        callback(resp);
        co_return;
      }

      auto resp = Utils::Common::makeHttpJsonResponse(k200OK, "Group deleted successfully.");
      callback(resp);
    }
    catch (const DrogonDbException &err)
    {
      auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "database error.");
      callback(resp);
    }
    co_return;
  }

  Task<void> Group::updateOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, ChatGroups::PrimaryKeyType &&id)
  {
    auto jsonPtr = req->getJsonObject();
    if (!jsonPtr)
    {
      auto resp = Utils::Common::makeHttpJsonResponse(k400BadRequest, "No json object is found in the request");
      co_return;
    }

    auto dbClientPtr = getDbClient();
    try
    {
      auto transPtr = co_await dbClientPtr->newTransactionCoro();
      ChatGroups object{id = id};
      object.setUpdatedat(trantor::Date::now());
      if ((*jsonPtr).isMember("name"))
        object.setName((*jsonPtr)["name"].asString());
      if ((*jsonPtr).isMember("avatar"))
        object.setName((*jsonPtr)["avatar"].asString());

      CoroMapper<ChatGroups> groupMapper(transPtr);
      auto count = co_await groupMapper.update(object);
      if (count != 1)
      {
        transPtr->rollback();
        auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "database error.");
        co_return;
      }

      // add group member
      if ((*jsonPtr).isMember("members") && (*jsonPtr)["members"].size())
      {
        CoroMapper<GroupMembers> memberMapper(transPtr);
        for (unsigned int index; index < (*jsonPtr)["members"].size(); ++index)
        {
          Json::Value memberObj;
          memberObj["groupId"] = id;
          memberObj["userId"] = (*jsonPtr)["members"][index].asInt64();
          try
          {
            auto member = co_await memberMapper.insert(GroupMembers(memberObj));
          }
          catch (const DrogonDbException &err)
          {
            LOG_INFO << err.base().what();
            transPtr->rollback();
            auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "database error.");
            callback(resp);
            co_return;
          }
        }
      }

      auto resp = Utils::Common::makeHttpJsonResponse(k200OK, "update successed.");
      callback(resp);
      co_return;
    }
    catch (const DrogonDbException &err)
    {
      LOG_INFO << err.base().what();
      auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "database error.");
      callback(resp);
      co_return;
    }
  }

  Task<void> Group::getOne(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback, ChatGroups::PrimaryKeyType &&id)
  {
    auto dbClientPtr = getDbClient();
    try
    {
      auto transPtr = co_await dbClientPtr->newTransactionCoro();
      CoroMapper<ChatGroups> mapper(transPtr);
      auto groupJson = (co_await mapper.findByPrimaryKey(id)).toJson();
      auto members = co_await getMembersWithInfo(transPtr, id);
      for (auto const &member : members)
      {
        auto memberJson = member.toJson();
        memberJson.removeMember("pwd");
        groupJson["members"].append(memberJson);
      }

      auto resp = Utils::Common::makeHttpJsonResponse(k200OK, "", groupJson);
      callback(resp);
    }
    catch (const DrogonDbException &err)
    {
      LOG_ERROR << err.base().what();
      auto resp = Utils::Common::makeHttpJsonResponse(k500InternalServerError, "database error.");
      callback(resp);
    }
    co_return;
  }

}
