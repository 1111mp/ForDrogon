#include "IMGroupCtrl.h"

namespace api::v1
{

  Task<void> Group::create(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
  {
    auto jsonPtr = *req->getJsonObject();
    auto type = jsonPtr["type"].asInt();
    auto members = jsonPtr["members"];

    if ((type != 1 && type != 2) || !members.isArray() || members.size() == 0)
    {
      Json::Value resultJson;
      resultJson["code"] = k400BadRequest;
      resultJson["msg"] = "Invalid parameter.";
      auto resp = HttpResponse::newHttpJsonResponse(resultJson);
      resp->setStatusCode(k400BadRequest);
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
        object["group_id"] = groupJson["id"].asInt64();
        object["user_id"] = members[index].asInt64();

        try
        {
          auto member = co_await memberMapper.insert(GroupMembers(object));

          auto id = member.getUserId().get();
          auto userJson = (co_await getUser(transPtr, *id)).toJson();
          userJson.removeMember("pwd");
          groupJson["members"].append(userJson);
        }
        catch (const DrogonDbException &err)
        {
          const orm::SqlError *s = dynamic_cast<const orm::SqlError *>(&err.base());
          Json::Value ret;
          if (s)
          {
            ret["code"] = k404NotFound;
            ret["msg"] = "Invalid userid: " + members[index].asString();
            auto resp = HttpResponse::newHttpJsonResponse(ret);
            resp->setStatusCode(k404NotFound);
            callback(resp);
            co_return;
          }
          LOG_ERROR << err.base().what();
          ret["code"] = k500InternalServerError;
          ret["msg"] = "database error.";
          auto resp = HttpResponse::newHttpJsonResponse(ret);
          resp->setStatusCode(k500InternalServerError);
          callback(resp);
          co_return;
        }
      }

      Json::Value resultJson;
      resultJson["code"] = k200OK;
      resultJson["data"] = groupJson;
      callback(HttpResponse::newHttpJsonResponse(resultJson));
    }
    catch (const Failure &err)
    {
      LOG_ERROR << err.what();
      Json::Value ret;
      ret["code"] = k500InternalServerError;
      ret["msg"] = "database error.";
      auto resp = HttpResponse::newHttpJsonResponse(ret);
      resp->setStatusCode(k500InternalServerError);
      callback(resp);
    }

    co_return;
  }

  void Group::deleteOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
  {
  }

  void Group::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
  {
  }

  void Group::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
  {
  }

}
