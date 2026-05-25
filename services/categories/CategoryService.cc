//
// Created by Emmanuel Addo-Odame on 16/05/2026.
//

#include "CategoryService.h"
#include "Categories.h"
#include "constants/ErrorCodes.h"
#include <json/json.h>

using namespace drogon::orm;
using drogon_model::Nos::Categories;

namespace nos::services {

drogon::Task<dto::BaseApiResponse> CategoryService::getAll(int pageNo, int pageSize, const std::string &query) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Categories>(dbClient);

    Criteria searchCriteria;
    if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria =
            Criteria(Categories::Cols::_name, CompareOperator::Like, likeQuery) ||
            Criteria(Categories::Cols::_description, CompareOperator::Like, likeQuery);
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
        auto categories = co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

        auto totalPages = (totalCount + pageSize - 1) / pageSize;

        response.success = true;
        response.result["totalCount"] = (Json::UInt64)totalCount;
        response.result["pageNo"] = pageNo;
        response.result["pageSize"] = pageSize;
        response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);
        response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
        response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount  : (Json::UInt64)(pageNo * pageSize);

        Json::Value data = Json::arrayValue;

        for (const auto &cat : categories) {

            Json::Value catJson = cat.toJson();
            Json::Value camelCaseCat;
            camelCaseCat["id"] = catJson["id"];
            camelCaseCat["name"] = catJson["name"];
            camelCaseCat["alias"] = catJson["alias"];
            camelCaseCat["notes"] = catJson["notes"];
            camelCaseCat["description"] = catJson["description"];
            camelCaseCat["parentId"] = catJson["parent_id"];
            camelCaseCat["status"] = catJson["status"];
            camelCaseCat["articleCount"] = catJson["article_count"];
            camelCaseCat["viewCount"] = catJson["view_count"];

            data.append(camelCaseCat);
        }
        response.result["data"] = data;

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.error["message"] = "Database error while fetching categories.";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> CategoryService::create(const dto::CategoryDto &categoryDto) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Categories>(dbClient);

    dto::BaseApiResponse response;
    try {
        Categories cat;
        cat.setId(drogon::utils::getUuid());
        cat.setName(categoryDto.getName());
        cat.setAlias(categoryDto.getAlias());
        cat.setNotes(categoryDto.getNotes());
        
        if (!categoryDto.getDescription().empty()) {
            cat.setDescription(categoryDto.getDescription());
        } else {
            cat.setDescriptionToNull();
        }
        
        if (!categoryDto.getParentId().empty()) {
            cat.setParentId(categoryDto.getParentId());
        } else {
            cat.setParentIdToNull();
        }

        cat.setStatus(categoryDto.getStatus());
        cat.setArticleCount(0);
        
        if (!categoryDto.getAccessLevelId().empty()) {
            cat.setAccessLevelId(categoryDto.getAccessLevelId());
        } else {
            cat.setAccessLevelIdToNull();
        }
        
        cat.setMetadesc(categoryDto.getMetadesc());
        cat.setMetakey(categoryDto.getMetakey());
        
        if (!categoryDto.getMetadata().empty()) {
            cat.setMetadata(categoryDto.getMetadata());
        } else {
            cat.setMetadataToNull();
        }

        cat.setViewCount(0);
        cat.setArticleCount(0);

        co_await mp.insert(cat);
        
        response.success = true;
        response.message = "Category created successfully";
        response.result["id"] = cat.getValueOfId();

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to create category";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> CategoryService::update(const dto::CategoryDto &categoryDto, const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Categories>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto cat = co_await mp.findOne(Criteria(Categories::Cols::_id, CompareOperator::EQ, id));

        if (!categoryDto.getName().empty()) {
            cat.setName(categoryDto.getName());
        }
        if (!categoryDto.getAlias().empty()) {
            cat.setAlias(categoryDto.getAlias());
        }
        if (!categoryDto.getNotes().empty()) {
            cat.setNotes(categoryDto.getNotes());
        }
        if (!categoryDto.getDescription().empty()) {
            cat.setDescription(categoryDto.getDescription());
        } else {
            cat.setDescriptionToNull();
        }
        if (!categoryDto.getParentId().empty()) {
            cat.setParentId(categoryDto.getParentId());
        } else {
            cat.setParentIdToNull();
        }

        cat.setStatus(categoryDto.getStatus());
        
        if (!categoryDto.getAccessLevelId().empty()) {
            cat.setAccessLevelId(categoryDto.getAccessLevelId());
        } else {
            cat.setAccessLevelIdToNull();
        }
        
        if (!categoryDto.getMetadesc().empty()) {
            cat.setMetadesc(categoryDto.getMetadesc());
        }
        if (!categoryDto.getMetakey().empty()) {
            cat.setMetakey(categoryDto.getMetakey());
        }
        if (!categoryDto.getMetadata().empty()) {
            cat.setMetadata(categoryDto.getMetadata());
        } else {
            cat.setMetadataToNull();
        }

        co_await mp.update(cat);

        response.success = true;
        response.message = "Category updated successfully";

    } catch (const UnexpectedRows &e) {
        response.success = false;
        response.message = "Category not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to update category";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> CategoryService::deleteCategory(const std::string &id) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Categories>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto count = co_await mp.deleteBy(Criteria(Categories::Cols::_id, CompareOperator::EQ, id));

        if (count > 0) {
            response.success = true;
            response.message = "Category deleted successfully";
        } else {
            response.success = false;
            response.message = "Category not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        }

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to delete category";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

}