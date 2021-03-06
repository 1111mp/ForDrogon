/**
 *
 *  RestfulChatGroupsCtrl.h
 *  This file is generated by drogon_ctl
 *
 */

#pragma once

#include <drogon/HttpController.h>
#include "RestfulChatGroupsCtrlBase.h"

#include "ChatGroups.h"
using namespace drogon;
using namespace drogon_model::database_test;
/**
 * @brief this class is created by the drogon_ctl command.
 * this class is a restful API controller for reading and writing the chat_groups table.
 */

class RestfulChatGroupsCtrl: public drogon::HttpController<RestfulChatGroupsCtrl>, public RestfulChatGroupsCtrlBase
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RestfulChatGroupsCtrl::getOne,"/chatgroups/{1}",Get,Options);
    ADD_METHOD_TO(RestfulChatGroupsCtrl::updateOne,"/chatgroups/{1}",Put,Options);
    ADD_METHOD_TO(RestfulChatGroupsCtrl::deleteOne,"/chatgroups/{1}",Delete,Options);
    ADD_METHOD_TO(RestfulChatGroupsCtrl::get,"/chatgroups",Get,Options);
    ADD_METHOD_TO(RestfulChatGroupsCtrl::create,"/chatgroups",Post,Options);
    //ADD_METHOD_TO(RestfulChatGroupsCtrl::update,"/chatgroups",Put,Options);
    METHOD_LIST_END
     
    void getOne(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                ChatGroups::PrimaryKeyType &&id);
    void updateOne(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   ChatGroups::PrimaryKeyType &&id);
    void deleteOne(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   ChatGroups::PrimaryKeyType &&id);
    void get(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback);

};
