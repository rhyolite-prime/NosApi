//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#ifndef NOSAPI_CONTENTSERVICE_H
#define NOSAPI_CONTENTSERVICE_H
#include "dto/BaseApiResponse.h"
#include <drogon/drogon.h>

#include "dto/DigitalContentDto.h"

namespace nos::services {

    class DigitalContentService {

    public:

        drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
        drogon::Task<dto::BaseApiResponse> getDetails(const std::string &id);
        drogon::Task<dto::BaseApiResponse> create(const dto::DigitalContentDto &digitalContentDto);
        drogon::Task<dto::BaseApiResponse> update(const dto::DigitalContentDto &digitalContentDto, const std::string &id);
        drogon::Task<dto::BaseApiResponse> deleteContent(const std::string &digitalContentId);

    };



}
#endif //NOSAPI_CONTENTSERVICE_H