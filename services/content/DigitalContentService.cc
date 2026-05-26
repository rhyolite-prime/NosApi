//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#include "DigitalContentService.h"
#include "DigitalContents.h"
#include "Workflows.h"
#include "WorkflowStages.h"
#include "WorkflowTransitions.h"
#include "WorkflowHistory.h"
#include "Users.h"
#include "constants/ErrorCodes.h"
#include <algorithm>
#include "constants/StatusTypes.h"
#include <cctype>
#include <cstring>
#include <json/json.h>

#include "plugins/NosApiPlugin.h"

using namespace drogon::orm;
using drogon_model::Nos::DigitalContents;
using drogon_model::Nos::Workflows;
using drogon_model::Nos::WorkflowStages;
using drogon_model::Nos::WorkflowTransitions;
using drogon_model::Nos::WorkflowHistory;
using drogon_model::Nos::Users;

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
      camelCaseContent["contentTypeName"] = contentJson["content_type_name"];
      camelCaseContent["introtext"] = contentJson["introtext"];
      camelCaseContent["fulltext"] = contentJson["fulltext"];
      camelCaseContent["status"] = contentJson["status"];
      camelCaseContent["categoryId"] = contentJson["category_id"];
      camelCaseContent["categoryName"] = contentJson["category_name"];
      camelCaseContent["ordering"] = contentJson["ordering"];
      camelCaseContent["featured"] = contentJson["featured"];
      camelCaseContent["accessLevelId"] = contentJson["access_level_id"];
      camelCaseContent["accessLevelName"] = contentJson["access_level_name"];
      camelCaseContent["author"] = contentJson["author"];
      camelCaseContent["currentWorkflowId"] = contentJson["current_workflow_id"];
      camelCaseContent["currentWorkflowName"] = contentJson["current_workflow_name"];
      camelCaseContent["currentWorkflowStageId"] = contentJson["current_workflow_stage_id"];
      camelCaseContent["currentWorkflowStageName"] = contentJson["current_workflow_stage_name"];
      camelCaseContent["nextWorkflowStageId"] = contentJson["next_workflow_stage_id"];
      camelCaseContent["nextWorkflowStageName"] = contentJson["next_workflow_stage_name"];



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
        reader.parse(contentJson["display_overrides"].asString(), displayOverridesJson);
      }
      camelCaseContent["displayOverrides"] = displayOverridesJson;


      camelCaseContent["version"] = contentJson["version"];
      camelCaseContent["language"] = contentJson["language"];

      camelCaseContent["views"] = contentJson["views"];

      data.append(camelCaseContent);
    }
    response.result["data"] = data;

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["message"] = "Database error while fetching digital contents.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}

drogon::Task<dto::BaseApiResponse> DigitalContentService::getDetails(const std::string &id) {

  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<DigitalContents>(dbClient);

  dto::BaseApiResponse response;
  try {

    auto content = co_await mp.findOne(Criteria(DigitalContents::Cols::_id, CompareOperator::EQ, id));

    Json::Value contentJson = content.toJson();
    Json::Value camelCaseContent;

    camelCaseContent["id"] = contentJson["id"];
    camelCaseContent["title"] = contentJson["title"];
    camelCaseContent["alias"] = contentJson["alias"];
    camelCaseContent["contentTypeId"] = contentJson["content_type_id"];
    camelCaseContent["contentTypeName"] = contentJson["content_type_name"];
    camelCaseContent["introtext"] = contentJson["introtext"];
    camelCaseContent["fulltext"] = contentJson["fulltext"];
    camelCaseContent["status"] = contentJson["status"];
    camelCaseContent["categoryId"] = contentJson["category_id"];
    camelCaseContent["categoryName"] = contentJson["category_name"];
    camelCaseContent["ordering"] = contentJson["ordering"];
    camelCaseContent["featured"] = contentJson["featured"];
    camelCaseContent["accessLevelId"] = contentJson["access_level_id"];
    camelCaseContent["accessLevelName"] = contentJson["access_level_name"];
    camelCaseContent["author"] = contentJson["author"];
    camelCaseContent["currentWorkflowId"] = contentJson["current_workflow_id"];
    camelCaseContent["currentWorkflowName"] = contentJson["current_workflow_name"];
    camelCaseContent["currentWorkflowStageId"] = contentJson["current_workflow_stage_id"];
    camelCaseContent["currentWorkflowStageName"] = contentJson["current_workflow_stage_name"];
    camelCaseContent["nextWorkflowStageId"] = contentJson["next_workflow_stage_id"];
    camelCaseContent["nextWorkflowStageName"] = contentJson["next_workflow_stage_name"];

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

    camelCaseContent["version"] = contentJson["version"];
    camelCaseContent["language"] = contentJson["language"];
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
    content.setCategoryName(digitalContentDto.getCategoryName());

    content.setOrdering(digitalContentDto.getOrdering());
    content.setIsFeatured(digitalContentDto.getFeatured());

    content.setAccessLevelId(digitalContentDto.getAccessLevelId());
    content.setAccessLevelName(digitalContentDto.getAccessLevelName());

    Json::FastWriter writer;
    if (digitalContentDto.getMetadata()) {
      std::string metadataStr = writer.write(digitalContentDto.getMetadata());
      if (!metadataStr.empty() && metadataStr.back() == '\n')
        metadataStr.pop_back();
      content.setMetadata(metadataStr);
    } else {
      content.setMetadata("{}");
    }

    content.setLanguage(digitalContentDto.getLanguage());

    content.setCreatedAt(trantor::Date::now());

    content.setModifiedAt(trantor::Date::now());
    content.setViews(0);

    // scan for the default, enabled workflow for that entity context
    auto workflowMp = CoroMapper<Workflows>(dbClient);
    auto workflows = co_await workflowMp.findBy(
        Criteria(Workflows::Cols::_context, CompareOperator::EQ, "com.digitalcontent.article") &&
        Criteria(Workflows::Cols::_is_default, CompareOperator::EQ, true) &&
        Criteria(Workflows::Cols::_status, CompareOperator::EQ, static_cast<int32_t>(constants::Enabled))
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

            auto transitionMp = CoroMapper<WorkflowTransitions>(dbClient);
            auto transitions = co_await transitionMp.orderBy(WorkflowTransitions::Cols::_ordering).limit(1).findBy(
                Criteria(WorkflowTransitions::Cols::_current_stage_id, CompareOperator::EQ, firstStage.getValueOfId()) &&
                Criteria(WorkflowTransitions::Cols::_workflow_id, CompareOperator::EQ, defaultWorkflow.getValueOfId())
            );

            if (!transitions.empty()) {
                auto firstTransition = transitions.front();
                auto nextStageId = firstTransition.getValueOfTargetStageId();
                content.setNextWorkflowStageId(nextStageId);
                
                auto nextStages = co_await stageMp.findBy(Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, nextStageId));
                if (!nextStages.empty()) {
                    content.setNextWorkflowStageName(nextStages.front().getValueOfTitle());
                }
            }
        }
    }

    co_await mp.insert(content);

    if (!workflows.empty()) {
        // handover to the workflow service to continue from here by passing the necessary params for code consistency
      auto plugin = drogon::app().getPlugin<nos::plugins::NosApiPlugin>();
      auto &workflowService = plugin->getWorkflowService();

        co_await workflowService.triggerWorkflow(content.getValueOfId(), content.getValueOfCurrentWorkflowId(), content.getValueOfCurrentWorkflowStageId());
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

drogon::Task<dto::BaseApiResponse> DigitalContentService::transitionContent(const std::string &contentId, const std::string &targetStageId) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<DigitalContents>(dbClient);
  auto stageMp = CoroMapper<WorkflowStages>(dbClient);
  
  dto::BaseApiResponse response;
  try {
    auto content = co_await mp.findOne(Criteria(DigitalContents::Cols::_id, CompareOperator::EQ, contentId));
    auto targetStage = co_await stageMp.findOne(Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, targetStageId));

    std::string previousStageId = content.getValueOfCurrentWorkflowStageId();

    content.setCurrentWorkflowStageId(targetStage.getValueOfId());
    content.setCurrentWorkflowStageName(targetStage.getValueOfTitle());

    auto transitionMp = CoroMapper<WorkflowTransitions>(dbClient);
    auto transitions = co_await transitionMp.orderBy(WorkflowTransitions::Cols::_ordering).limit(1).findBy(
        Criteria(WorkflowTransitions::Cols::_current_stage_id, CompareOperator::EQ, targetStage.getValueOfId()) &&
        Criteria(WorkflowTransitions::Cols::_workflow_id, CompareOperator::EQ, targetStage.getValueOfWorkflowId())
    );

    if (!transitions.empty()) {
        auto nextTransition = transitions.front();
        auto nextStageId = nextTransition.getValueOfTargetStageId();
        content.setNextWorkflowStageId(nextStageId);
        
        auto nextStage = co_await stageMp.findOne(Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, nextStageId));
        content.setNextWorkflowStageName(nextStage.getValueOfTitle());
    } else {
        content.setNextWorkflowStageIdToNull();
        content.setNextWorkflowStageName("-");
    }
    
    content.setModifiedAt(trantor::Date::now());

    co_await mp.update(content);

    // Record workflow history
    auto historyMp = CoroMapper<WorkflowHistory>(dbClient);
    WorkflowHistory history;
    history.setContentId(contentId);
    if (!previousStageId.empty()) {
        history.setCurrentStageId(previousStageId);
    }
    history.setTargetStageId(targetStageId);
    
    auto currentTransitions = co_await transitionMp.findBy(
        Criteria(WorkflowTransitions::Cols::_current_stage_id, CompareOperator::EQ, previousStageId) &&
        Criteria(WorkflowTransitions::Cols::_target_stage_id, CompareOperator::EQ, targetStageId)
    );
    if (!currentTransitions.empty()) {
        history.setTransitionId(currentTransitions.front().getValueOfId());
    }
    
    history.setCreatedAt(trantor::Date::now());
    history.setModifiedAt(trantor::Date::now());
    co_await historyMp.insert(history);

    // Try sending email notification if we can find the author
    if (content.getCreatedBy() && !content.getValueOfCreatedBy().empty()) {
      auto userMp = CoroMapper<Users>(dbClient);
      try {
        auto user = co_await userMp.findOne(Criteria(Users::Cols::_id, CompareOperator::EQ, content.getValueOfCreatedBy()));
        if (user.getEmail() && !user.getValueOfEmail().empty()) {
          auto plugin = drogon::app().getPlugin<nos::plugins::NosApiPlugin>();
          auto &emailService = plugin->getEmailService();

          dto::SendEmailDto emailDto;
          emailDto.setTo(user.getValueOfEmail());
          emailDto.setSubject("Content Workflow Transition: " + content.getValueOfTitle());
          
          std::string htmlBody = 
              "<html><body style=\"font-family: Arial, sans-serif; line-height: 1.6; color: #333;\">"
              "<div style=\"max-width: 600px; margin: auto; padding: 20px; border: 1px solid #ddd; border-radius: 8px; background-color: #f9f9f9;\">"
              "<h2 style=\"color: #0056b3;\">Workflow Transition Notification</h2>"
              "<p>Hello,</p>"
              "<p>The digital content <strong>" + content.getValueOfTitle() + "</strong> has been transitioned to a new stage.</p>"
              "<div style=\"padding: 15px; background-color: #ffffff; border-left: 4px solid #0056b3; margin: 20px 0;\">"
              "  <p style=\"margin: 0;\"><strong>New Stage:</strong> " + targetStage.getValueOfTitle() + "</p>"
              "</div>"
              "<p>Please log in to the system to review the content.</p>"
              "<hr style=\"border: 0; border-top: 1px solid #eee; margin: 20px 0;\">"
              "<p style=\"font-size: 0.9em; color: #777;\">This is an automated notification, please do not reply.</p>"
              "</div></body></html>";
              
          emailDto.setBody(htmlBody);
          //co_await emailService.sendEmail(emailDto);
        }
      } catch (const UnexpectedRows &e) {
        // User not found, ignore
      }
    }

    response.success = true;
    response.message = "Digital content transitioned successfully";
    
  } catch (const UnexpectedRows &e) {
    response.success = false;
    response.message = "Content or target stage not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Database error while transitioning content";
    response.error["detail"] = e.base().what();
  }
  
  co_return response;
}
  
} // namespace nos::services