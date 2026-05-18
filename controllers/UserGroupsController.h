#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class UserGroupsController : public drogon::HttpController<UserGroupsController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/user-groups/";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserGroupsController::getAll, std::string(PREFIX) + "get-all", Get, Options,);
    ADD_METHOD_TO(UserGroupsController::createUserGroup, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(UserGroupsController::updateUserGroup,std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
    ADD_METHOD_TO(UserGroupsController::deleteUserGroup, std::string(PREFIX) + "delete/{1}", Delete, Options);
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    Task<HttpResponsePtr> createUserGroup(HttpRequestPtr req);
    Task<HttpResponsePtr> updateUserGroup(HttpRequestPtr req, std::string userGroupId);
    Task<HttpResponsePtr> deleteUserGroup(HttpRequestPtr req, std::string userGroupId);
};
