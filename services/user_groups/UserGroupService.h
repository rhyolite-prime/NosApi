//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#ifndef NOSAPI_USERGROUPSERVICE_H
#define NOSAPI_USERGROUPSERVICE_H
#include <drogon/drogon.h>
#include "dto/BaseApiResponse.h"
#include "dto/UserGroupDto.h"

namespace nos::services {

    class UserGroupService {

    public:
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::UserGroupDto &userGroupDto);

        drogon::Task<dto::BaseApiResponse> update(const dto::UserGroupDto &userGroupDto, const std::string &userGroupId);

        drogon::Task<dto::BaseApiResponse> deleteGroup(const std::string &userGroupId);

    };


}
#endif //NOSAPI_USERGROUPSERVICE_H