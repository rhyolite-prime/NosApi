//
// Created by Emmanuel Addo-Odame on 25/05/2026.
//

#include "ContentTypeService.h"
#include "ContentTypes.h"
#include "constants/ErrorCodes.h"
#include <json/json.h>

using namespace drogon::orm;
using drogon_model::Nos::ContentTypes;

namespace nos::services {

drogon::Task<dto::BaseApiResponse> ContentTypeService::getAll(int pageNo, int pageSize, const std::string &query) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<ContentTypes>(dbClient);

    Criteria searchCriteria;
    if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria =
            Criteria(ContentTypes::Cols::_name, CompareOperator::Like, likeQuery) ||
            Criteria(ContentTypes::Cols::_description, CompareOperator::Like, likeQuery);
    }

    dto::BaseApiResponse response;
    try {
        size_t totalCount = co_await mp.count(searchCriteria);
        if (totalCount == 0) {
            response.success = true;
            response.result["data"] = Json::arrayValue;
            response.result["totalCount"] = 0;
            co_return response;
        }

        int offset = (pageNo - 1) * pageSize;
        auto contentTypes = co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

        auto totalPages = (totalCount + pageSize - 1) / pageSize;

        response.success = true;
        response.result["totalCount"] = (Json::UInt64)totalCount;
        response.result["pageNo"] = pageNo;
        response.result["pageSize"] = pageSize;
        response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);
        response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
        response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount  : (Json::UInt64)(pageNo * pageSize);

        Json::Value data = Json::arrayValue;

        for (const auto &ct : contentTypes) {
            Json::Value ctJson = ct.toJson();
            Json::Value camelCaseCt;
            camelCaseCt["id"] = ctJson["id"];
            camelCaseCt["name"] = ctJson["name"];
            camelCaseCt["description"] = ctJson["description"];
            camelCaseCt["iconClass"] = ctJson["icon_class"];
            camelCaseCt["allowedExtensions"] = ctJson["allowed_extensions"];

            Json::Value allowedExtensionsArray;
            Json::Reader reader;
            if (ctJson.isMember("allowed_extensions") && ctJson["allowed_extensions"].isString()) {
                reader.parse(ctJson["allowed_extensions"].asString(), allowedExtensionsArray);
            }
            camelCaseCt["allowedExtensions"] = allowedExtensionsArray.isArray() ? allowedExtensionsArray : Json::arrayValue;


            camelCaseCt["maxFileSize"] = ctJson["max_file_size"];
            camelCaseCt["requiresMedia"] = ctJson["requires_media"];
            camelCaseCt["requiresThumbnail"] = ctJson["requires_thumbnail"];
            camelCaseCt["sortOrder"] = ctJson["sort_order"];
            camelCaseCt["isActive"] = ctJson["is_active"];
            camelCaseCt["isSystem"] = ctJson["is_system"];
            //camelCaseCt["metadataSchema"] = ctJson["metadata_schema"];

            
            data.append(camelCaseCt);
        }
        response.result["data"] = data;

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.error["message"] = "Database error while fetching content types.";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> ContentTypeService::create(const dto::ContentTypeDto &contentTypeDto) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<ContentTypes>(dbClient);

    dto::BaseApiResponse response;
    try {
        ContentTypes ct;
        ct.setId(drogon::utils::getUuid());
        ct.setName(contentTypeDto.getName());
        
        if (!contentTypeDto.getDescription().empty()) {
            ct.setDescription(contentTypeDto.getDescription());
        } else {
            ct.setDescriptionToNull();
        }
        
        if (!contentTypeDto.getIconClass().empty()) {
            ct.setIconClass(contentTypeDto.getIconClass());
        } else {
            ct.setIconClassToNull();
        }
        
        ct.setAllowedExtensions(contentTypeDto.getAllowedExtensions());
        ct.setMaxFileSize(contentTypeDto.getMaxFileSize());
        ct.setRequiresMedia(contentTypeDto.getRequiresMedia());
        ct.setRequiresThumbnail(contentTypeDto.getRequiresThumbnail());
        ct.setSortOrder(contentTypeDto.getSortOrder());
        ct.setIsActive(contentTypeDto.getIsActive());
        ct.setIsSystem(contentTypeDto.getIsSystem());
        
        if (!contentTypeDto.getMetadataSchema().empty()) {
            ct.setMetadataSchema(contentTypeDto.getMetadataSchema());
        } else {
            ct.setMetadataSchemaToNull();
        }
        
        ct.setCreatedByToNull();
        ct.setCreatedAt(trantor::Date::now());
        ct.setModifiedByToNull();
        ct.setModifiedAt(trantor::Date::now());

        co_await mp.insert(ct);
        
        response.success = true;
        response.message = "Content type created successfully";
        response.result["id"] = ct.getValueOfId();

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to create content type";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> ContentTypeService::update(const dto::ContentTypeDto &contentTypeDto, const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<ContentTypes>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto ct = co_await mp.findOne(Criteria(ContentTypes::Cols::_id, CompareOperator::EQ, id));

        if (!contentTypeDto.getName().empty()) {
            ct.setName(contentTypeDto.getName());
        }
        if (!contentTypeDto.getDescription().empty()) {
            ct.setDescription(contentTypeDto.getDescription());
        } else {
            ct.setDescriptionToNull();
        }
        if (!contentTypeDto.getIconClass().empty()) {
            ct.setIconClass(contentTypeDto.getIconClass());
        } else {
            ct.setIconClassToNull();
        }
        if (!contentTypeDto.getAllowedExtensions().empty()) {
            ct.setAllowedExtensions(contentTypeDto.getAllowedExtensions());
        }
        
        ct.setMaxFileSize(contentTypeDto.getMaxFileSize());
        ct.setRequiresMedia(contentTypeDto.getRequiresMedia());
        ct.setRequiresThumbnail(contentTypeDto.getRequiresThumbnail());
        ct.setSortOrder(contentTypeDto.getSortOrder());
        ct.setIsActive(contentTypeDto.getIsActive());
        ct.setIsSystem(contentTypeDto.getIsSystem());
        
        if (!contentTypeDto.getMetadataSchema().empty()) {
            ct.setMetadataSchema(contentTypeDto.getMetadataSchema());
        } else {
            ct.setMetadataSchemaToNull();
        }

        ct.setModifiedAt(trantor::Date::now());

        co_await mp.update(ct);

        response.success = true;
        response.message = "Content type updated successfully";

    } catch (const UnexpectedRows &e) {
        response.success = false;
        response.message = "Content type not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to update content type";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> ContentTypeService::deleteContentType(const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<ContentTypes>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto count = co_await mp.deleteBy(Criteria(ContentTypes::Cols::_id, CompareOperator::EQ, id));

        if (count > 0) {
            response.success = true;
            response.message = "Content type deleted successfully";
        } else {
            response.success = false;
            response.message = "Content type not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        }

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to delete content type";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

}