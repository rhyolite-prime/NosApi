#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class RolesController : public drogon::HttpController<RolesController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/roles/";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RolesController::getAll, std::string(PREFIX) + "get-all", Get, Options,);
    ADD_METHOD_TO(RolesController::createRole, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(RolesController::updateRole,std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
    ADD_METHOD_TO(RolesController::deleteRole, std::string(PREFIX) + "delete/{1}", Delete, Options);
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    Task<HttpResponsePtr> createRole(HttpRequestPtr req);
    Task<HttpResponsePtr> updateRole(HttpRequestPtr req, std::string roleId);
    Task<HttpResponsePtr> deleteRole(HttpRequestPtr req, std::string roleId);
};
