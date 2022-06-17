#include "IMGroupCtrl.h"

namespace api::v1
{

  void Group::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
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
      return callback(resp);
    }

    auto dbClientPtr = getDbClient();
    dbClientPtr->newTransactionAsync([=](const std::shared_ptr<orm::Transaction> &transPtr)
                                     {
                                       auto userID = req->getAttributes()->get<int64_t>("jwt_userid");
                                       Mapper<ChatGroups> mapper(transPtr);
                                       auto chatGroup = ChatGroups(jsonPtr);
                                       chatGroup.setCreator(userID);
                                       chatGroup.setMax(type == 1 ? 200 : 2000);
                                       mapper.insert(
                                           chatGroup,
                                           [=](const ChatGroups &group)
                                           {
                                             auto groupJson = group.toJson();
                                             groupJson["members"] = Json::arrayValue;
                                             

                                             
                                             Mapper<GroupMembers> memberMapper(transPtr);
                                             for (int index = 0; index < members.size(); ++index)
                                             {
                                               Json::Value object;
                                               object["group_id"] = groupJson["id"].asInt64();
                                               object["user_id"] = members[index].asInt64();
                                               memberMapper.insert(
                                                   GroupMembers(object),
                                                   [&, transPtr](const GroupMembers &member)
                                                   {
                                                     member.getUser(
                                                      transPtr, 
                                                      [&](const Users &user) {
                                                        auto userJson = user.toJson();
                                                        LOG_INFO << userJson["id"].asInt64();
                                                        groupJson["members"].append(userJson);
                                                      }, 
                                                      [=](const DrogonDbException &err) {
                                                        // const orm::UnexpectedRows *s = dynamic_cast<const orm::UnexpectedRows *>(&err.base());
                                                        // Json::Value ret;
                                                        // if (s)
                                                        // {
                                                        //   ret["code"] = k404NotFound;
                                                        //   ret["msg"] = "Invalid userid: " + members[index].asString();
                                                        //   auto resp = HttpResponse::newHttpJsonResponse(ret);
                                                        //   resp->setStatusCode(k404NotFound);
                                                        //   return callback(resp);
                                                        // }
                                                        
                                                        // LOG_ERROR << err.base().what();
                                                        // ret["code"] = k500InternalServerError;
                                                        // ret["msg"] = "database error";
                                                        // auto resp = HttpResponse::newHttpJsonResponse(ret);
                                                        // resp->setStatusCode(k500InternalServerError);
                                                        // return callback(resp);
                                                      }
                                                     );
                                                   },
                                                   [=](const DrogonDbException &err)
                                                   {
                                                    const orm::SqlError *s = dynamic_cast<const orm::SqlError *>(&err.base());
                                                        Json::Value ret;
                                                        if (s)
                                                        {
                                                          ret["code"] = k404NotFound;
                                                          ret["msg"] = "Invalid userid: " + members[index].asString();
                                                          auto resp = HttpResponse::newHttpJsonResponse(ret);
                                                          resp->setStatusCode(k404NotFound);
                                                          return callback(resp);
                                                        }
                                                     LOG_ERROR << err.base().what();
                                                     ret["code"] = k500InternalServerError;
                                                     ret["msg"] = "database errors";
                                                     auto resp = HttpResponse::newHttpJsonResponse(ret);
                                                     resp->setStatusCode(k500InternalServerError);
                                                     return callback(resp);
                                                   });
                                             }

                                              Json::Value resultJson;
                                              resultJson["code"] = k200OK;
                                              resultJson["data"] = groupJson;
                                              return callback(HttpResponse::newHttpJsonResponse(resultJson));
                                           },
                                           [=](const DrogonDbException &err)
                                           {
                                             LOG_ERROR << err.base().what();
                                             Json::Value ret;
                                             ret["code"] = k500InternalServerError;
                                             ret["msg"] = "database error";
                                             auto resp = HttpResponse::newHttpJsonResponse(ret);
                                             resp->setStatusCode(k500InternalServerError);
                                             return callback(resp);
                                           }); });
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
