#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class DigitalContentController : public drogon::HttpController<DigitalContentController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/digital-content/";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(DigitalContentController::getAll, std::string(PREFIX) + "get-all", Get, Options, "JwtAuthFilter");
    ADD_METHOD_TO(DigitalContentController::getContentDetails, std::string(PREFIX) + "{1}", Get, Options, "JwtAuthFilter");
    ADD_METHOD_TO(DigitalContentController::createDigitalContent, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(DigitalContentController::updateDigitalContent,std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
    ADD_METHOD_TO(DigitalContentController::deleteDigitalContent, std::string(PREFIX) + "delete/{1}", Delete, Options, "JwtAuthFilter");
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    Task<HttpResponsePtr> getContentDetails(HttpRequestPtr req, std::string id);
    Task<HttpResponsePtr> createDigitalContent(HttpRequestPtr req);
    Task<HttpResponsePtr> updateDigitalContent(HttpRequestPtr req, std::string id);
    Task<HttpResponsePtr> deleteDigitalContent(HttpRequestPtr req, std::string id);
};
