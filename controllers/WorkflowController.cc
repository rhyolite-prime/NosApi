#include "WorkflowController.h"

#include "plugins/NosApiPlugin.h"

Task<HttpResponsePtr> WorkflowController::getAll(HttpRequestPtr req) {
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
    auto &workflowService = plugin->getWorkflowService();

    auto result = co_await workflowService.getAll(pageNo, pageSize, query);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
    co_return resp;
}

Task<HttpResponsePtr> WorkflowController::createWorkflow(HttpRequestPtr req) {
    auto jsonBody = req->getJsonObject();
    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::WorkflowDto dto;
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
    auto &workflowService = plugin->getWorkflowService();

    auto result = co_await workflowService.create(dto);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : k500InternalServerError);
    co_return resp;
}

Task<HttpResponsePtr> WorkflowController::updateWorkflow(HttpRequestPtr req, std::string userId) {
    auto jsonBody = req->getJsonObject();
    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::WorkflowDto dto;
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
    auto &workflowService = plugin->getWorkflowService();

    auto result = co_await workflowService.update(dto, userId);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    bool notFound = !result.success && result.error.isMember("code") && result.error["code"].asInt() == 1004;
    resp->setStatusCode(result.success ? k200OK : (notFound ? k404NotFound : k500InternalServerError));
    co_return resp;
}

Task<HttpResponsePtr> WorkflowController::deleteWorkflow(HttpRequestPtr req, std::string workflowId) {

    auto plugin = app().getPlugin<nos::plugins::NosApiPlugin>();
    auto &workflowService = plugin->getWorkflowService();

    auto result = co_await workflowService.deleteWorkflow(workflowId);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    bool notFound = !result.success && result.error.isMember("code") && result.error["code"].asInt() == 1004;
    resp->setStatusCode(result.success ? k200OK : (notFound ? k404NotFound : k500InternalServerError));
    co_return resp;
}