//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#include "RoleService.h"
#include "Roles.h"
#include "constants/ErrorCodes.h"
#include <json/json.h>

using namespace drogon::orm;
using drogon_model::Nos::Roles;

namespace nos::services {

drogon::Task<dto::BaseApiResponse> RoleService::getAll(int pageNo, int pageSize, const std::string &query) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Roles>(dbClient);

    Criteria searchCriteria;
    if (!query.empty()) {
        std::string likeQuery = "%" + query + "%";
        searchCriteria =
            Criteria(Roles::Cols::_name, CompareOperator::Like, likeQuery) ||
            Criteria(Roles::Cols::_description, CompareOperator::Like, likeQuery);
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
        auto roles = co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

        auto totalPages = (totalCount + pageSize - 1) / pageSize;

        response.success = true;
        response.result["totalCount"] = (Json::UInt64)totalCount;
        response.result["pageNo"] = pageNo;
        response.result["pageSize"] = pageSize;
        response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);
        response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
        response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount  : (Json::UInt64)(pageNo * pageSize);

        Json::Value data = Json::arrayValue;

        for (const auto &role : roles) {
            Json::Value roleJson = role.toJson();
            Json::Value camelCaseRole;
            camelCaseRole["id"] = roleJson["id"];
            camelCaseRole["name"] = roleJson["name"];
            camelCaseRole["description"] = roleJson["description"];
            
            Json::Value permissionsArray;
            Json::Reader reader;
            if (roleJson.isMember("permissions") && roleJson["permissions"].isString()) {
                reader.parse(roleJson["permissions"].asString(), permissionsArray);
            }
            camelCaseRole["permissions"] = permissionsArray.isArray() ? permissionsArray : Json::arrayValue;
            
            // camelCaseRole["createdBy"] = roleJson["created_by"];
            // camelCaseRole["createdAt"] = roleJson["created_at"];
            // camelCaseRole["modifiedBy"] = roleJson["modified_by"];
            // camelCaseRole["modifiedAt"] = roleJson["modified_at"];
            data.append(camelCaseRole);
        }
        response.result["data"] = data;

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.error["message"] = "Database error while fetching roles.";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> RoleService::create(const dto::RoleDto &roleDto) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Roles>(dbClient);

    dto::BaseApiResponse response;
    try {
        Roles role;
        role.setName(roleDto.getName());
        role.setDescription(roleDto.getDescription());
        
        Json::Value permissionsArray = Json::arrayValue;
        for (const auto& perm : roleDto.getPermissions()) {
            permissionsArray.append(perm);
        }
        Json::FastWriter writer;
        std::string permissionsStr = writer.write(permissionsArray);
        if (!permissionsStr.empty() && permissionsStr.back() == '\n') {
            permissionsStr.pop_back();
        }
        role.setPermissions(permissionsStr);
        
        co_await mp.insert(role);
        
        response.success = true;
        response.message = "Role created successfully";

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to create role";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> RoleService::update(const dto::RoleDto &roleDto, const std::string &roleId) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Roles>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto role = co_await mp.findOne(Criteria(Roles::Cols::_id, CompareOperator::EQ, roleId));

        if (!roleDto.getName().empty()) {
            role.setName(roleDto.getName());
        }
        if (!roleDto.getDescription().empty()) {
            role.setDescription(roleDto.getDescription());
        }
        if (!roleDto.getPermissions().empty()) {
            Json::Value permissionsArray = Json::arrayValue;
            for (const auto& perm : roleDto.getPermissions()) {
                permissionsArray.append(perm);
            }
            Json::FastWriter writer;
            std::string permissionsStr = writer.write(permissionsArray);
            if (!permissionsStr.empty() && permissionsStr.back() == '\n') {
                permissionsStr.pop_back();
            }
            role.setPermissions(permissionsStr);
        }

        role.setModifiedAt(trantor::Date::now());

        co_await mp.update(role);

        response.success = true;
        response.message = "Role updated successfully";

    } catch (const UnexpectedRows &e) {
        response.success = false;
        response.message = "Role not found";
        response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to update role";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

drogon::Task<dto::BaseApiResponse> RoleService::deleteRole(const std::string &roleId) {
    auto dbClient = drogon::app().getDbClient();
    auto mp = CoroMapper<Roles>(dbClient);

    dto::BaseApiResponse response;
    try {
        auto count = co_await mp.deleteBy(Criteria(Roles::Cols::_id, CompareOperator::EQ, roleId));

        if (count > 0) {
            response.success = true;
            response.message = "Role deleted successfully";
        } else {
            response.success = false;
            response.message = "Role not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        }

    } catch (const DrogonDbException &e) {
        response.success = false;
        response.message = "Failed to delete role";
        response.error["detail"] = e.base().what();
    }
    co_return response;
}

}