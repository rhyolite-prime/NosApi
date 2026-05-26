#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class CategoriesController : public drogon::HttpController<CategoriesController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/categories/";
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CategoriesController::getAll, std::string(PREFIX) + "get-all", Get, Options, "JwtAuthFilter");
    ADD_METHOD_TO(CategoriesController::createCategory, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
    ADD_METHOD_TO(CategoriesController::updateCategory, std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
    ADD_METHOD_TO(CategoriesController::deleteCategory, std::string(PREFIX) + "delete/{1}", Delete, Options, "JwtAuthFilter");
    METHOD_LIST_END

    Task<HttpResponsePtr> getAll(HttpRequestPtr req);
    Task<HttpResponsePtr> createCategory(HttpRequestPtr req);
    Task<HttpResponsePtr> updateCategory(HttpRequestPtr req, std::string categoryId);
    Task<HttpResponsePtr> deleteCategory(HttpRequestPtr req, std::string categoryId);
};
