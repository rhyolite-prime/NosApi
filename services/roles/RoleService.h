//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#ifndef NOSAPI_ROLESERVICE_H
#define NOSAPI_ROLESERVICE_H
#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include "dto/RoleDto.h"

namespace nos::services {

    class RoleService {

        public:
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::RoleDto &roleDto);

        drogon::Task<dto::BaseApiResponse> update(const dto::RoleDto &roleDto, const std::string &roleId);

        drogon::Task<dto::BaseApiResponse> deleteRole(const std::string &roleId);
    };



}
#endif //NOSAPI_ROLESERVICE_H