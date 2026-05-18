#include "RolesController.h"
#include "dto/BaseApiResponse.h"
#include "services/roles/RoleService.h"

Task<HttpResponsePtr> RolesController::getAll(HttpRequestPtr req) {
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

    nos::services::RoleService roleService;
    auto result = co_await roleService.getAll(pageNo, pageSize, query);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
    co_return resp;
}

Task<HttpResponsePtr> RolesController::createRole(HttpRequestPtr req) {
    auto jsonBody = req->getJsonObject();
    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::RoleDto dto;
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

    nos::services::RoleService roleService;
    auto result = co_await roleService.create(dto);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
    co_return resp;
}

Task<HttpResponsePtr> RolesController::updateRole(HttpRequestPtr req, std::string roleId) {
    auto jsonBody = req->getJsonObject();
    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::RoleDto dto;
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

    nos::services::RoleService roleService;
    auto result = co_await roleService.update(dto, roleId);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    bool notFound = !result.success && result.error.isMember("code") && result.error["code"].asInt() == 1004;
    resp->setStatusCode(result.success ? k200OK : (notFound ? k404NotFound : k500InternalServerError));
    co_return resp;
}

Task<HttpResponsePtr> RolesController::deleteRole(HttpRequestPtr req, std::string roleId) {
    nos::services::RoleService roleService;
    auto result = co_await roleService.deleteRole(roleId);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    bool notFound = !result.success && result.error.isMember("code") && result.error["code"].asInt() == 1004;
    resp->setStatusCode(result.success ? k200OK : (notFound ? k404NotFound : k500InternalServerError));
    co_return resp;
}
