#include "UserGroupService.h"
#include "UserGroups.h"
#include "constants/ErrorCodes.h"
#include <json/json.h>
#include <algorithm>
#include <cctype>

using namespace drogon::orm;
using drogon_model::Nos::UserGroups;

namespace nos::services {

inline std::string titleToLtreeLabel(const std::string &title) {
  std::string label = "";
  for (char c : title) {
    if (std::isalnum(c)) {
      label += std::tolower(c);
    } else if (c == ' ' || c == '_' || c == '-') {
      if (label.empty() || label.back() != '_') {
        label += '_';
      }
    }
  }
  if (!label.empty() && label.back() == '_') {
    label.pop_back();
  }
  if (label.empty()) {
    label = "group";
  }
  return label;
}

drogon::Task<dto::BaseApiResponse> UserGroupService::getAll(int pageNo, int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<UserGroups>(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(UserGroups::Cols::_title, CompareOperator::Like, likeQuery);
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
    auto groups =
        co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount  : (Json::UInt64)(pageNo * pageSize);


    Json::Value data = Json::arrayValue;

    for (const auto &group : groups) {
      Json::Value groupJson = group.toJson();
      Json::Value camelCaseGroup;
      camelCaseGroup["id"] = groupJson["id"];
      camelCaseGroup["title"] = groupJson["title"];
      camelCaseGroup["parentId"] = groupJson["parent_id"];
      camelCaseGroup["path"] = groupJson["path"];
      
      if (group.getRoles()) {
        Json::Value rolesJson;
        Json::Reader reader;
        if (reader.parse(group.getValueOfRoles(), rolesJson)) {
          camelCaseGroup["roles"] = rolesJson;
        } else {
          camelCaseGroup["roles"] = group.getValueOfRoles();
        }
      } else {
        camelCaseGroup["roles"] = Json::arrayValue;
      }

      camelCaseGroup["activeUsers"] = groupJson["active_users"];
      camelCaseGroup["blockedUsers"] = groupJson["blocked_users"];
      camelCaseGroup["createdBy"] = groupJson["created_by"];
      camelCaseGroup["createdAt"] = groupJson["created_at"];
      camelCaseGroup["modifiedBy"] = groupJson["modified_by"];
      camelCaseGroup["modifiedAt"] = groupJson["modified_at"];
      data.append(camelCaseGroup);
    }
    response.result["data"] = data;

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["message"] = "Database error while fetching user groups.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserGroupService::create(const dto::UserGroupDto &userGroupDto) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<UserGroups>(dbClient);

  dto::BaseApiResponse response;
  try {
    size_t count = co_await mp.count(Criteria(UserGroups::Cols::_title, CompareOperator::EQ, userGroupDto.getTitle()));
    if (count > 0) {
      response.success = false;
      response.message = "User group with this name already exists";
      response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
      co_return response;
    }

    std::string title = userGroupDto.getTitle();
    std::string parentId = userGroupDto.getParentId();
    std::string rolesStr = "";
    bool hasRoles = false;
    if (userGroupDto.hasRoles() && !userGroupDto.getRoles().empty()) {
      hasRoles = true;
      Json::Value rolesJson = Json::arrayValue;
      for (const auto &item : userGroupDto.getRoles()) {
        Json::Value r;
        r["id"] = item.id;
        r["name"] = item.name;
        rolesJson.append(r);
      }
      Json::FastWriter writer;
      rolesStr = writer.write(rolesJson);
    }

    auto now = trantor::Date::now();
    int32_t zero = 0;

    auto doInsert = [&](const std::string &t, const std::string &p, const std::string &r, bool hr) -> drogon::Task<drogon::orm::Result> {
      if (!p.empty()) {
        if (hr) {
          co_return co_await dbClient->execSqlCoro(
              "INSERT INTO user_groups (title, parent_id, roles, active_users, blocked_users, created_at, modified_at) VALUES ($1, $2, $3, $4, $5, $6, $7) RETURNING id",
              t, p, r, zero, zero, now, now
          );
        } else {
          co_return co_await dbClient->execSqlCoro(
              "INSERT INTO user_groups (title, parent_id, active_users, blocked_users, created_at, modified_at) VALUES ($1, $2, $3, $4, $5, $6) RETURNING id",
              t, p, zero, zero, now, now
          );
        }
      } else {
        if (hr) {
          co_return co_await dbClient->execSqlCoro(
              "INSERT INTO user_groups (title, roles, active_users, blocked_users, created_at, modified_at) VALUES ($1, $2, $3, $4, $5, $6) RETURNING id",
              t, r, zero, zero, now, now
          );
        } else {
          co_return co_await dbClient->execSqlCoro(
              "INSERT INTO user_groups (title, active_users, blocked_users, created_at, modified_at) VALUES ($1, $2, $3, $4, $5) RETURNING id",
              t, zero, zero, now, now
          );
        }
      }
    };

    drogon::orm::Result insertResult = co_await doInsert(title, parentId, rolesStr, hasRoles);

    if (insertResult.empty()) {
      response.success = false;
      response.message = "Failed to retrieve generated group ID";
      co_return response;
    }

    std::string generatedId = insertResult[0]["id"].as<std::string>();

    // Calculate hierarchical path
    std::string childPath;
    if (!parentId.empty()) {
      try {
        auto parent = co_await mp.findOne(Criteria(UserGroups::Cols::_id, CompareOperator::EQ, parentId));
        std::string parentPath = parent.getValueOfPath();
        if (parentPath.empty()) {
          parentPath = titleToLtreeLabel(parent.getValueOfTitle());
        }
        childPath = parentPath + "." + titleToLtreeLabel(title);
      } catch (...) {
        childPath = titleToLtreeLabel(title);
      }
    } else {
      childPath = titleToLtreeLabel(title);
    }

    co_await dbClient->execSqlCoro(
        "UPDATE user_groups SET path = $1::ltree WHERE id = $2::uuid",
        childPath, generatedId
    );

    response.success = true;
    response.message = "User group created successfully";
    response.result["userGroupId"] = generatedId;

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to create user group";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserGroupService::update(const dto::UserGroupDto &userGroupDto, const std::string &userGroupId) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<UserGroups>(dbClient);

  dto::BaseApiResponse response;
  try {
    auto group = co_await mp.findOne(Criteria(UserGroups::Cols::_id, CompareOperator::EQ, userGroupId));

    std::string oldPath = group.getValueOfPath();
    std::string newPath = oldPath;

    // Detect if title is updated and we need to apply it
    bool titleChanged = false;
    if (!userGroupDto.getTitle().empty() && userGroupDto.getTitle() != group.getValueOfTitle()) {
      size_t nameCount = co_await mp.count(Criteria(UserGroups::Cols::_title, CompareOperator::EQ, userGroupDto.getTitle()));
      if (nameCount > 0) {
        response.success = false;
        response.message = "User group with this name already exists";
        response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
        co_return response;
      }
      group.setTitle(userGroupDto.getTitle());
      titleChanged = true;
    }

    // Handle roles update
    if (userGroupDto.hasRoles()) {
      if (!userGroupDto.getRoles().empty()) {
        Json::Value rolesJson = Json::arrayValue;
        for (const auto &item : userGroupDto.getRoles()) {
          Json::Value r;
          r["id"] = item.id;
          r["name"] = item.name;
          rolesJson.append(r);
        }
        Json::FastWriter writer;
        group.setRoles(writer.write(rolesJson));
      } else {
        group.setRolesToNull();
      }
    }

    // Handle path hierarchy update if parentId is changed
    bool parentChanged = false;
    if (userGroupDto.hasParentId()) {
      std::string newParentId = userGroupDto.getParentId();
      std::string oldParentId = group.getValueOfParentId();
      if (newParentId != oldParentId) {
        parentChanged = true;
        if (!newParentId.empty()) {
          group.setParentId(newParentId);
        } else {
          group.setParentIdToNull();
        }
      }
    }

    // If either title or parent changed, recalculate the path
    if (titleChanged || parentChanged) {
      std::string currentParentId = group.getValueOfParentId();
      std::string currentTitle = group.getValueOfTitle();
      
      if (!currentParentId.empty()) {
        try {
          auto parent = co_await mp.findOne(Criteria(UserGroups::Cols::_id, CompareOperator::EQ, currentParentId));
          std::string parentPath = parent.getValueOfPath();
          if (parentPath.empty()) {
            parentPath = titleToLtreeLabel(parent.getValueOfTitle());
          }
          newPath = parentPath + "." + titleToLtreeLabel(currentTitle);
        } catch (...) {
          newPath = titleToLtreeLabel(currentTitle);
          group.setParentIdToNull();
        }
      } else {
        newPath = titleToLtreeLabel(currentTitle);
      }
      
      group.setPath(newPath);

      // Recursively update all descendants paths in PostgreSQL
      if (!oldPath.empty() && oldPath != newPath) {
        co_await dbClient->execSqlCoro(
            "UPDATE user_groups SET path = $1::ltree || subpath(path, nlevel($2::ltree)) WHERE path <@ $2::ltree AND id <> $3::uuid",
            newPath, oldPath, userGroupId
        );
      }
    }

    group.setModifiedAt(trantor::Date::now());

    co_await mp.update(group);

    response.success = true;
    response.message = "User group updated successfully";

  } catch (const UnexpectedRows &e) {
    response.success = false;
    response.message = "User group not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to update user group";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> UserGroupService::deleteGroup(const std::string &userGroupId) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<UserGroups>(dbClient);

  dto::BaseApiResponse response;
  try {
    auto count = co_await mp.deleteBy(Criteria(UserGroups::Cols::_id, CompareOperator::EQ, userGroupId));

    if (count > 0) {
      response.success = true;
      response.message = "User group deleted successfully";
    } else {
      response.success = false;
      response.message = "User group not found";
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    }

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to delete user group";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

}