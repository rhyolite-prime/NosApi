#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AccessLevelsController : public drogon::HttpController<AccessLevelsController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/access-level/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AccessLevelsController::getAll, std::string(PREFIX) + "get-all", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AccessLevelsController::createAccessLevel, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AccessLevelsController::updateAccessLevel,std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
  ADD_METHOD_TO(AccessLevelsController::deleteAccessLevel, std::string(PREFIX) + "delete/{1}", Delete, Options, "JwtAuthFilter");
  METHOD_LIST_END

  Task<HttpResponsePtr> getAll(HttpRequestPtr req);
  Task<HttpResponsePtr> createAccessLevel(HttpRequestPtr req);
  Task<HttpResponsePtr> updateAccessLevel(HttpRequestPtr req, std::string accessLevelId);
  Task<HttpResponsePtr> deleteAccessLevel(HttpRequestPtr req, std::string accessLevelId);
};
