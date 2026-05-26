//
// Created by Emmanuel Addo-Odame on 25/05/2026.
//

#ifndef NOSAPI_CONTENTTYPESERVICE_H
#define NOSAPI_CONTENTTYPESERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

#include "dto/ContentTypeDto.h"


namespace nos::services {

    class ContentTypeService {

    public:
        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);

        drogon::Task<dto::BaseApiResponse> create(const dto::ContentTypeDto &contentTypeDto);

        drogon::Task<dto::BaseApiResponse> update(const dto::ContentTypeDto &contentTypeDto, const std::string &id);

        drogon::Task<dto::BaseApiResponse> deleteContentType(const std::string &id);

    };

}

#endif //NOSAPI_CONTENTTYPESERVICE_H