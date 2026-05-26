//
// Created by Emmanuel Addo-Odame on 13/05/2026.
//

#ifndef NOSAPI_CATEGORYSERVICE_H
#define NOSAPI_CATEGORYSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

#include "dto/CategoryDto.h"

namespace nos::services {

    class CategoryService {

    public:
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::CategoryDto &categoryDto);

        drogon::Task<dto::BaseApiResponse> update(const dto::CategoryDto &categoryDto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deleteCategory(const std::string &id);

    };


}
#endif //NOSAPI_CATEGORYSERVICE_H