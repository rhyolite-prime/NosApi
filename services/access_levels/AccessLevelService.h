//
// Created by Emmanuel Addo-Odame on 16/05/2026.
//

#ifndef NOSAPI_ACCESSLEVELSERVICE_H
#define NOSAPI_ACCESSLEVELSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

#include "dto/AccessLevelDto.h"

namespace nos::services {

    class AccessLevelService {

    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::AccessLevelDto &accessLevelDto);

        drogon::Task<dto::BaseApiResponse> update(const dto::AccessLevelDto &accessLevelDto, const std::string &accessLevelId);

        drogon::Task<dto::BaseApiResponse> deleteAccessLevel(const std::string &accessLevelId);

    };


}

#endif //NOSAPI_ACCESSLEVELSERVICE_H