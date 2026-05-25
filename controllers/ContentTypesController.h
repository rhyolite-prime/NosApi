#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class ContentTypesController : public drogon::HttpController<ContentTypesController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/content-types/";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ContentTypesController::getAll, std::string(PREFIX) + "get-all", Get, Options, "JwtAuthFilter");
    ADD_METHOD_TO(ContentTypesController::createContentType, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(ContentTypesController::updateContentType, std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
    ADD_METHOD_TO(ContentTypesController::deleteContentType, std::string(PREFIX) + "delete/{1}", Delete, Options, "JwtAuthFilter");
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    Task<HttpResponsePtr> createContentType(HttpRequestPtr req);
    Task<HttpResponsePtr> updateContentType(HttpRequestPtr req, std::string contentTypeId);
    Task<HttpResponsePtr> deleteContentType(HttpRequestPtr req, std::string contentTypeId);
};
