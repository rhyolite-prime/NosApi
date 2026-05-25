#include "CategoriesController.h"
#include "dto/BaseApiResponse.h"
#include "plugins/NosApiPlugin.h"
#include "services/categories/CategoryService.h"

Task<HttpResponsePtr> CategoriesController::getAll(HttpRequestPtr req) {

    int pageNo = 1;
    int pageSize = 10;
    std::string query = "";

    auto pageNoStr = req->getParameter("pageNo");
    if (!pageNoStr.empty()) {
        try { pageNo = std::stoi(pageNoStr); } catch (...) {}
    }

    auto pageSizeStr = req->getParameter("pageSize");
    if (!pageSizeStr.empty()) {
        try { pageSize = std::stoi(pageSizeStr); } catch (...) {}
    }

    query = req->getParameter("query");

    auto plugin = app().getPlugin<nos::plugins::NosApiPlugin>();
    auto &categoryService = plugin->getCategoryService();

    auto result = co_await categoryService.getAll(pageNo, pageSize, query);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
    co_return resp;
}

Task<HttpResponsePtr> CategoriesController::createCategory(HttpRequestPtr req) {
    auto jsonBody = req->getJsonObject();
    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::CategoryDto dto;
    try {
        dto.fromJson(*jsonBody);
    } catch (const std::exception &e) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Missing or invalid required fields";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = app().getPlugin<nos::plugins::NosApiPlugin>();
    auto &categoryService = plugin->getCategoryService();

    auto result = co_await categoryService.create(dto);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
    co_return resp;
}

Task<HttpResponsePtr> CategoriesController::updateCategory(HttpRequestPtr req, std::string categoryId) {
    auto jsonBody = req->getJsonObject();
    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::CategoryDto dto;
    try {
        dto.fromJson(*jsonBody);
    } catch (const std::exception &e) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Missing or invalid required fields";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    auto plugin = app().getPlugin<nos::plugins::NosApiPlugin>();
    auto &categoryService = plugin->getCategoryService();

    auto result = co_await categoryService.update(dto, categoryId);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    bool notFound = !result.success && result.error.isMember("code") && result.error["code"].asInt() == 1004;
    resp->setStatusCode(result.success ? k200OK : (notFound ? k404NotFound : k500InternalServerError));
    co_return resp;
}

Task<HttpResponsePtr> CategoriesController::deleteCategory(HttpRequestPtr req, std::string categoryId) {
    auto plugin = app().getPlugin<nos::plugins::NosApiPlugin>();
    auto &categoryService = plugin->getCategoryService();

    auto result = co_await categoryService.deleteCategory(categoryId);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    bool notFound = !result.success && result.error.isMember("code") && result.error["code"].asInt() == 1004;
    resp->setStatusCode(result.success ? k200OK : (notFound ? k404NotFound : k500InternalServerError));
    co_return resp;
}
