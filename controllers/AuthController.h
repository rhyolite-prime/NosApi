#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController>
{
  public:
    static constexpr const char *PREFIX = "/api/v1/auth/";
    METHOD_LIST_BEGIN
    //ADD_METHOD_TO(AuthController::checkAccountStatus, std::string(PREFIX) + "check-account-status", Get, Options);
    ADD_METHOD_TO(AuthController::loginViaPasskeys, std::string(PREFIX) + "login-via-pass-keys", Post, Options);
    ADD_METHOD_TO(AuthController::registerPasskeys,std::string(PREFIX) + "register-pass-keys", Post, Options);
    ADD_METHOD_TO(AuthController::adminSignIn, std::string(PREFIX) + "admin-login", Post, Options);
    METHOD_LIST_END

    //Task<HttpResponsePtr> checkAccountStatus(HttpRequestPtr req);
    Task<HttpResponsePtr> registerPasskeys(HttpRequestPtr req);
    //Task<HttpResponsePtr> signIn(HttpRequestPtr req);
    Task<HttpResponsePtr> loginViaPasskeys(HttpRequestPtr req);
    Task<HttpResponsePtr> adminSignIn(HttpRequestPtr req);

private:
    void setAuthCookie(const HttpResponsePtr &resp, const std::string &token);
};
