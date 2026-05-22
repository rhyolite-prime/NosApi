//
// Created by Emmanuel Addo-Odame on 16/05/2026.
//

#include "AccessLevelService.h"
#include "AccessLevels.h"
#include "constants/ErrorCodes.h"
#include <json/json.h>

using namespace drogon::orm;
using drogon_model::Nos::AccessLevels;

namespace nos::services {

drogon::Task<dto::BaseApiResponse> AccessLevelService::getAll(int pageNo, int pageSize, const std::string &query) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<AccessLevels>(dbClient);

    Criteria searchCriteria;
    if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria = Criteria(AccessLevels::Cols::_title, CompareOperator::Like, likeQuery);
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
        auto accessLevels = co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

        auto totalPages = (totalCount + pageSize - 1) / pageSize;

        response.success = true;
        response.result["totalCount"] = (Json::UInt64)totalCount;
        response.result["pageNo"] = pageNo;
        response.result["pageSize"] = pageSize;
        response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);
        response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
        response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount  : (Json::UInt64)(pageNo * pageSize);

        Json::Value data = Json::arrayValue;

        for (const auto &accessLevel : accessLevels) {
            Json::Value accessLevelJson = accessLevel.toJson();
            Json::Value camelCaseAccessLevel;
            camelCaseAccessLevel["id"] = accessLevelJson["id"];
            camelCaseAccessLevel["title"] = accessLevelJson["title"];
            camelCaseAccessLevel["ordering"] = accessLevelJson["ordering"];
            
            Json::Value rulesArray;
            Json::Reader reader;
            if (accessLevelJson.isMember("rules") && accessLevelJson["rules"].isString()) {
                reader.parse(accessLevelJson["rules"].asString(), rulesArray);
            }
            camelCaseAccessLevel["rules"] = rulesArray.isArray() ? rulesArray : Json::arrayValue;
            
            data.append(camelCaseAccessLevel);
        }
        response.result["data"] = data;

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.error["message"] = "Database error while fetching access levels.";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> AccessLevelService::create(const dto::AccessLevelDto &accessLevelDto) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<AccessLevels>(dbClient);

    dto::BaseApiResponse response;
    try {

        AccessLevels accessLevel;
        accessLevel.setTitle(accessLevelDto.getTitle());
        accessLevel.setOrdering(accessLevelDto.getOrdering());

        Json::Value rulesArray = Json::arrayValue;
        for (const auto& rule : accessLevelDto.getRules()) {
            rulesArray.append(rule);
        }
        Json::FastWriter writer;
        std::string rulesStr = writer.write(rulesArray);
        if (!rulesStr.empty() && rulesStr.back() == '\n') {
            rulesStr.pop_back();
        }
        accessLevel.setRules(rulesStr);
        
        co_await mp.insert(accessLevel);
        
        response.success = true;
        response.message = "Access level created successfully";

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to create access level";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> AccessLevelService::update(const dto::AccessLevelDto &accessLevelDto, const std::string &accessLevelId) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<AccessLevels>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto accessLevel = co_await mp.findOne(Criteria(AccessLevels::Cols::_id, CompareOperator::EQ, accessLevelId));

        accessLevel.setTitle(accessLevelDto.getTitle());
        accessLevel.setOrdering(accessLevelDto.getOrdering());

        if (!accessLevelDto.getRules().empty()) {
            Json::Value rulesArray = Json::arrayValue;
            for (const auto& rule : accessLevelDto.getRules()) {
                rulesArray.append(rule);
            }
            Json::FastWriter writer;
            std::string rulesStr = writer.write(rulesArray);
            if (!rulesStr.empty() && rulesStr.back() == '\n') {
                rulesStr.pop_back();
            }
            accessLevel.setRules(rulesStr);
        }else {
            accessLevel.setRulesToNull();
        }

        co_await mp.update(accessLevel);

        response.success = true;
        response.message = "Access level updated successfully";

    } catch (const UnexpectedRows &e) {
        response.success = false;
        response.message = "Access level not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to update access level";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> AccessLevelService::deleteAccessLevel(const std::string &accessLevelId) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<AccessLevels>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto count = co_await mp.deleteBy(Criteria(AccessLevels::Cols::_id, CompareOperator::EQ, accessLevelId));

        if (count > 0) {
            response.success = true;
            response.message = "Access level deleted successfully";
        } else {
            response.success = false;
            response.message = "Access level not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        }

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to delete access level";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

}