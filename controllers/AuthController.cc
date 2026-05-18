#include "AuthController.h"
#include "dto/BaseApiResponse.h"
#include <string>

#include "dto/RegisterUserPasskeysDto.h"
#include "dto/SigninDto.h"
#include "plugins/NosApiPlugin.h"
#include "services/users/UserService.h"


Task<HttpResponsePtr> AuthController::adminSignIn(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::SigninDto signin_dto;

    try {
        signin_dto.fromJson(*jsonBody);
    } catch (const std::exception &e) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Missing or invalid required fields";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }


    auto plugin = app().getPlugin<nos::plugins::NosApiPlugin>();
    auto &userService = plugin->getUserService();

    auto result = co_await userService.validateAdminUserCredentials(signin_dto);

    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());
    resp->setStatusCode(result.success ? k200OK : k500InternalServerError);

    if (result.success) {
        setAuthCookie(resp, result.result["token"].asString());
    }

    co_return resp;
}

Task<HttpResponsePtr> AuthController::loginViaPasskeys(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::UserPasskeyDto dto;
    dto.fromJson(*jsonBody);

    auto plugin = app().getPlugin<nos::plugins::NosApiPlugin>();
    auto &userService = plugin->getUserService();

    auto result = co_await userService.validateUserPasskeys(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

    if (result.success && result.result.isMember("token")) {
        setAuthCookie(resp, result.result["token"].asString());
    }

    co_return resp;
}



drogon::Task<HttpResponsePtr> AuthController::registerPasskeys(HttpRequestPtr req) {

    auto jsonBody = req->getJsonObject();

    if (!jsonBody) {
        nos::dto::BaseApiResponse response;
        response.success = false;
        response.error["message"] = "Invalid JSON body";
        auto resp = HttpResponse::newHttpJsonResponse(response.toJson());
        resp->setStatusCode(k400BadRequest);
        co_return resp;
    }

    nos::dto::RegisterUserPasskeysDto dto;
    dto.fromJson(*jsonBody);

    // Get tenant service from plugin
    auto plugin = app().getPlugin<nos::plugins::NosApiPlugin>();
    auto &userService = plugin->getUserService();

    auto result = co_await userService.registerUserPasskeys(dto);
    auto resp = HttpResponse::newHttpJsonResponse(result.toJson());

    if (result.success && result.result.isMember("token")) {
        setAuthCookie(resp, result.result["token"].asString());
    }

    co_return resp;
}



void AuthController::setAuthCookie(const HttpResponsePtr &resp,
                                   const std::string &token) {
    drogon::Cookie cookie("auth_token", token);
    cookie.setHttpOnly(true);
    cookie.setSecure(true);
    cookie.setPath("/");
    // 24 * 120 hours = 120 days. Match the token expiration.
    cookie.setMaxAge(24 * 120 * 3600);
    cookie.setSameSite(drogon::Cookie::SameSite::kLax);
    resp->addCookie(cookie);
}
