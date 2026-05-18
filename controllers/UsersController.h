#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class UsersController : public drogon::HttpController<UsersController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/users/";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UsersController::getAll, std::string(PREFIX) + "get-all", Get, Options,);
    ADD_METHOD_TO(UsersController::createUser, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(UsersController::updateUser,std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
    ADD_METHOD_TO(UsersController::deleteUser, std::string(PREFIX) + "delete/{1}", Delete, Options);
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    Task<HttpResponsePtr> createUser(HttpRequestPtr req);
    Task<HttpResponsePtr> updateUser(HttpRequestPtr req, std::string userId);
    Task<HttpResponsePtr> deleteUser(HttpRequestPtr req, std::string userId);

};
