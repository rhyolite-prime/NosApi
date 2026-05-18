//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#ifndef NOSAPI_USERSERVICE_H
#define NOSAPI_USERSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/SigninDto.h"
#include "dto/UserPasskeyDto.h"
#include <drogon/drogon.h>
#include "dto/RegisterUserPasskeysDto.h"
#include "dto/UserDto.h"

namespace nos::services {

    class UserService {

      public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::UserDto &userDto);

        drogon::Task<dto::BaseApiResponse> update(const dto::UserDto &userDto, const std::string &userId);

        drogon::Task<dto::BaseApiResponse> deleteUser(const std::string &userId);

        drogon::Task<dto::BaseApiResponse> registerUserPasskeys(const dto::RegisterUserPasskeysDto &passKeysDto);

        drogon::Task<dto::BaseApiResponse> validateUserCredentials(const dto::SigninDto &signin_dto);

        drogon::Task<dto::BaseApiResponse> validateUserPasskeys(const dto::UserPasskeyDto &passkeyDto);

        drogon::Task<dto::BaseApiResponse> validateAdminUserCredentials(const dto::SigninDto &signin_dto);

    };

}
#endif //NOSAPI_USERSERVICE_H