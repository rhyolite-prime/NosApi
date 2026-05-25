//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#include "DigitalContentService.h"
#include "DigitalContents.h"
#include "Workflows.h"
#include "WorkflowStages.h"
#include "../work_flows/WorkflowService.h"
#include "constants/ErrorCodes.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <json/json.h>

using namespace drogon::orm;
using drogon_model::Nos::DigitalContents;
using drogon_model::Nos::Workflows;
using drogon_model::Nos::WorkflowStages;

namespace nos::services {

inline std::string generateAlias(const std::string &title) {
  std::string alias = "";
  for (char c : title) {
    if (std::isalnum(c)) {
      alias += std::tolower(c);
    } else if (c == ' ' || c == '_' || c == '-') {
      if (alias.empty() || alias.back() != '-') {
        alias += '-';
      }
    }
  }
  if (!alias.empty() && alias.back() == '-') {
    alias.pop_back();
  }
  if (alias.empty()) {
    alias = "content";
  }
  return alias;
}

drogon::Task<dto::BaseApiResponse> DigitalContentService::getAll(int pageNo, int pageSize,
                              const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<DigitalContents>(dbClient);

  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria = Criteria(DigitalContents::Cols::_title,
                              CompareOperator::Like, likeQuery) ||
                     Criteria(DigitalContents::Cols::_introtext,
                              CompareOperator::Like, likeQuery) ||
                     Criteria(DigitalContents::Cols::_fulltext,
                              CompareOperator::Like, likeQuery);
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
    auto contents =
        co_await mp.limit(pageSize)
            .offset(offset)
            .orderBy(DigitalContents::Cols::_ordering, SortOrder::ASC)
            .findBy(searchCriteria);

    auto totalPages = (totalCount + pageSize - 1) / pageSize;

    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] =
        (int)((totalCount + pageSize - 1) / pageSize);
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo
                                        ? (Json::UInt64)totalCount
                                        : (Json::UInt64)(pageNo * pageSize);

    Json::Value data = Json::arrayValue;
    for (const auto &content : contents) {
      Json::Value contentJson = content.toJson();
      Json::Value camelCaseContent;
      camelCaseContent["id"] = contentJson["id"];
      camelCaseContent["title"] = contentJson["title"];
      camelCaseContent["alias"] = contentJson["alias"];
      camelCaseContent["contentTypeId"] = contentJson["content_type_id"];
      camelCaseContent["contentType"] = contentJson["content_type"];
      camelCaseContent["introtext"] = contentJson["introtext"];
      camelCaseContent["fulltext"] = contentJson["fulltext"];
      camelCaseContent["status"] = contentJson["status"];
      camelCaseContent["categoryId"] = contentJson["category_id"];
      camelCaseContent["ordering"] = contentJson["ordering"];
      camelCaseContent["featured"] = contentJson["featured"];
      camelCaseContent["accessLevelId"] = contentJson["access_level_id"];


      Json::Reader reader;
      Json::Value metadataJson = Json::objectValue;
      if (contentJson.isMember("metadata") &&
          contentJson["metadata"].isString()) {
        reader.parse(contentJson["metadata"].asString(), metadataJson);
      }
      camelCaseContent["metadata"] = metadataJson;

      Json::Value presentationConfigJson = Json::objectValue;
      if (contentJson.isMember("presentation_config") &&
          contentJson["presentation_config"].isString()) {
        reader.parse(contentJson["presentation_config"].asString(),
                     presentationConfigJson);
      }
      camelCaseContent["presentationConfig"] = presentationConfigJson;

      Json::Value displayOverridesJson = Json::objectValue;
      if (contentJson.isMember("display_overrides") &&
          contentJson["display_overrides"].isString()) {
        reader.parse(contentJson["display_overrides"].asString(),
                     displayOverridesJson);
      }
      camelCaseContent["displayOverrides"] = displayOverridesJson;

      camelCaseContent["publishUp"] = contentJson["publish_up"];
      camelCaseContent["publishDown"] = contentJson["publish_down"];
      camelCaseContent["version"] = contentJson["version"];
      camelCaseContent["language"] = contentJson["language"];
      camelCaseContent["createdBy"] = contentJson["created_by"];
      camelCaseContent["createdAt"] = contentJson["created_at"];
      camelCaseContent["modifiedBy"] = contentJson["modified_by"];
      camelCaseContent["modifiedAt"] = contentJson["modified_at"];
      camelCaseContent["views"] = contentJson["views"];

      data.append(camelCaseContent);
    }
    response.result["data"] = data;

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["message"] =
        "Database error while fetching digital contents.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse>
DigitalContentService::getDetails(const std::string &id) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<DigitalContents>(dbClient);

  dto::BaseApiResponse response;
  try {
    auto content = co_await mp.findOne(
        Criteria(DigitalContents::Cols::_id, CompareOperator::EQ, id));

    Json::Value contentJson = content.toJson();
    Json::Value camelCaseContent;
    camelCaseContent["id"] = contentJson["id"];
    camelCaseContent["title"] = contentJson["title"];
    camelCaseContent["alias"] = contentJson["alias"];
    camelCaseContent["contentTypeId"] = contentJson["content_type_id"];
    camelCaseContent["contentType"] = contentJson["content_type"];
    camelCaseContent["introtext"] = contentJson["introtext"];
    camelCaseContent["fulltext"] = contentJson["fulltext"];
    camelCaseContent["primaryResourceId"] = contentJson["primary_resource_id"];
    camelCaseContent["status"] = contentJson["status"];
    camelCaseContent["categoryId"] = contentJson["category_id"];
    camelCaseContent["ordering"] = contentJson["ordering"];
    camelCaseContent["featured"] = contentJson["featured"];
    camelCaseContent["accessLevelId"] = contentJson["access_level_id"];
    camelCaseContent["metakey"] = contentJson["metakey"];
    camelCaseContent["metadesc"] = contentJson["metadesc"];

    Json::Reader reader;
    Json::Value metadataJson = Json::objectValue;
    if (contentJson.isMember("metadata") &&
        contentJson["metadata"].isString()) {
      reader.parse(contentJson["metadata"].asString(), metadataJson);
    }
    camelCaseContent["metadata"] = metadataJson;

    Json::Value presentationConfigJson = Json::objectValue;
    if (contentJson.isMember("presentation_config") &&
        contentJson["presentation_config"].isString()) {
      reader.parse(contentJson["presentation_config"].asString(),
                   presentationConfigJson);
    }
    camelCaseContent["presentationConfig"] = presentationConfigJson;

    Json::Value displayOverridesJson = Json::objectValue;
    if (contentJson.isMember("display_overrides") &&
        contentJson["display_overrides"].isString()) {
      reader.parse(contentJson["display_overrides"].asString(),
                   displayOverridesJson);
    }
    camelCaseContent["displayOverrides"] = displayOverridesJson;

    camelCaseContent["publishUp"] = contentJson["publish_up"];
    camelCaseContent["publishDown"] = contentJson["publish_down"];
    camelCaseContent["version"] = contentJson["version"];
    camelCaseContent["language"] = contentJson["language"];
    camelCaseContent["createdBy"] = contentJson["created_by"];
    camelCaseContent["createdAt"] = contentJson["created_at"];
    camelCaseContent["modifiedBy"] = contentJson["modified_by"];
    camelCaseContent["modifiedAt"] = contentJson["modified_at"];
    camelCaseContent["views"] = contentJson["views"];

    response.success = true;
    response.result["data"] = camelCaseContent;

  } catch (const UnexpectedRows &e) {
    response.success = false;
    response.message = "Digital content not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Database error while fetching digital content details.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> DigitalContentService::create(const dto::DigitalContentDto &digitalContentDto) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<DigitalContents>(dbClient);

  dto::BaseApiResponse response;
  try {
    std::string alias =!digitalContentDto.getAlias().empty() ? digitalContentDto.getAlias() : generateAlias(digitalContentDto.getTitle());

    size_t count = co_await mp.count(Criteria(DigitalContents::Cols::_alias, CompareOperator::EQ, alias));

    if (count > 0) {
      response.success = false;
      response.message = "Digital content with this alias already exists";
      response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
      co_return response;
    }

    DigitalContents content;
    content.setTitle(digitalContentDto.getTitle());
    content.setAlias(alias);

    content.setContentTypeId(digitalContentDto.getContentTypeId());

    content.setContentTypeName(digitalContentDto.getContentTypeName());

    content.setIntrotext(digitalContentDto.getIntrotext());

    content.setFulltext(digitalContentDto.getFulltext());

    content.setStatus(digitalContentDto.getStatus());

    content.setCategoryId(digitalContentDto.getCategoryId());

    content.setOrdering(digitalContentDto.getOrdering());
    content.setIsFeatured(digitalContentDto.getFeatured());

    content.setAccessLevelId(digitalContentDto.getAccessLevelId());

    // Json::FastWriter writer;
    // if (digitalContentDto.getMetadata()) {
    //   std::string metadataStr = writer.write(digitalContentDto.getMetadata());
    //   if (!metadataStr.empty() && metadataStr.back() == '\n')
    //     metadataStr.pop_back();
    //   content.setMetadata(metadataStr);
    // } else {
    //   content.setMetadata("{}");
    // }

    content.setLanguage(digitalContentDto.getLanguage());

    content.setCreatedAt(trantor::Date::now());

    content.setModifiedAt(trantor::Date::now());
    content.setViews(digitalContentDto.getViews());

    // scan for the default, enabled workflow for that entity context
    auto workflowMp = CoroMapper<Workflows>(dbClient);
    auto workflows = co_await workflowMp.findBy(
        Criteria(Workflows::Cols::_context, CompareOperator::EQ, "com.digitalcontent.article") &&
        Criteria(Workflows::Cols::_is_default, CompareOperator::EQ, true) &&
        Criteria(Workflows::Cols::_status, CompareOperator::EQ, 1)
    );

    if (!workflows.empty()) {
        auto defaultWorkflow = workflows.front();
        content.setCurrentWorkflowId(defaultWorkflow.getValueOfId());
        content.setCurrentWorkflowName(defaultWorkflow.getValueOfTitle());

        auto stageMp = CoroMapper<WorkflowStages>(dbClient);
        auto stages = co_await stageMp.findBy(
            Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, defaultWorkflow.getValueOfId()) &&
            Criteria(WorkflowStages::Cols::_is_default, CompareOperator::EQ, true)
        );

        if (stages.empty()) {
            stages = co_await stageMp.orderBy(WorkflowStages::Cols::_ordering).limit(1).findBy(
                Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, defaultWorkflow.getValueOfId())
            );
        }

        if (!stages.empty()) {
            auto firstStage = stages.front();
            content.setCurrentWorkflowStageId(firstStage.getValueOfId());
            content.setCurrentWorkflowStageName(firstStage.getValueOfTitle());
        }
    }

    co_await mp.insert(content);

    if (!workflows.empty()) {
        // handover to the workflow service to continue from here by passing the necessary params for code consistency
        WorkflowService workflowService;
        co_await workflowService.triggerWorkflow(
            content.getValueOfId(),
            content.getValueOfCurrentWorkflowId(),
            content.getValueOfCurrentWorkflowStageId()
        );
    }

    response.success = true;
    response.message = "Digital content created successfully";
    response.result["digitalContentId"] = content.getValueOfId();

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to create digital content";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}



drogon::Task<dto::BaseApiResponse> DigitalContentService::update(const dto::DigitalContentDto &digitalContentDto,
                              const std::string &id) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<DigitalContents>(dbClient);

  dto::BaseApiResponse response;
  try {
    if (id.empty()) {
      response.success = false;
      response.message = "Digital content ID is required for update";
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
      co_return response;
    }

    auto content = co_await mp.findOne(Criteria(DigitalContents::Cols::_id, CompareOperator::EQ, id));

      content.setTitle(digitalContentDto.getTitle());

      std::string alias = digitalContentDto.getAlias().empty() ? generateAlias(content.getValueOfTitle()) : digitalContentDto.getAlias();

      if (alias != content.getValueOfAlias()) {
        size_t count = co_await mp.count(Criteria(DigitalContents::Cols::_alias, CompareOperator::EQ, alias));
        if (count > 0) {
          response.success = false;
          response.message = "Digital content with this alias already exists";
          response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
          co_return response;
        }
      }

    content.setAlias(alias);

    content.setContentTypeId(digitalContentDto.getContentTypeId());

    content.setContentTypeName(digitalContentDto.getContentTypeName());

    content.setIntrotext(digitalContentDto.getIntrotext());

    content.setFulltext(digitalContentDto.getFulltext());

    content.setStatus(digitalContentDto.getStatus());

    content.setCategoryId(digitalContentDto.getCategoryId());
    content.setOrdering(digitalContentDto.getOrdering());
    content.setIsFeatured(digitalContentDto.getFeatured());

    content.setAccessLevelId(digitalContentDto.getAccessLevelId());


    content.setVersion(content.getValueOfVersion());
    content.setLanguage(digitalContentDto.getLanguage());
    content.setViews(digitalContentDto.getViews());

    content.setModifiedAt(trantor::Date::now());

    co_await mp.update(content);

    response.success = true;
    response.message = "Digital content updated successfully";

  } catch (const UnexpectedRows &e) {
    response.success = false;
    response.message = "Digital content not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to update digital content";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> DigitalContentService::deleteContent(const std::string &digitalContentId) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<DigitalContents>(dbClient);

  dto::BaseApiResponse response;
  try {
    auto count = co_await mp.deleteBy(Criteria(
        DigitalContents::Cols::_id, CompareOperator::EQ, digitalContentId));

    if (count > 0) {
      response.success = true;
      response.message = "Digital content deleted successfully";
    } else {
      response.success = false;
      response.message = "Digital content not found";
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    }

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to delete digital content";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

} // namespace nos::services