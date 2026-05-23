//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//
#include "WorkflowService.h"
#include "Workflows.h"
#include "WorkflowStages.h"
#include "WorkflowTransitions.h"
#include "constants/ErrorCodes.h"
#include <json/json.h>
#include <algorithm>

using namespace drogon::orm;
using drogon_model::Nos::Workflows;
using drogon_model::Nos::WorkflowStages;
using drogon_model::Nos::WorkflowTransitions;

namespace nos::services {

    drogon::Task<dto::BaseApiResponse> WorkflowService::getAll(int pageNo, int pageSize, const std::string &query) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<Workflows>(dbClient);

        Criteria searchCriteria;
        if (!query.empty()) {
            std::string likeQuery = "%" + query + "%";
            searchCriteria =
                Criteria(Workflows::Cols::_title, CompareOperator::Like, likeQuery) ||
                Criteria(Workflows::Cols::_description, CompareOperator::Like, likeQuery);
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
            auto workflows = co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

            auto totalPages = (totalCount + pageSize - 1) / pageSize;

            response.success = true;
            response.result["totalCount"] = (Json::UInt64)totalCount;
            response.result["pageNo"] = pageNo;
            response.result["pageSize"] = pageSize;
            response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);
            response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
            response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount  : (Json::UInt64)(pageNo * pageSize);

            Json::Value data = Json::arrayValue;
            for (const auto &w : workflows) {
                Json::Value wJson = w.toJson();
                Json::Value camelCaseW;
                camelCaseW["id"] = wJson["id"];
                camelCaseW["status"] = wJson["status"];
                camelCaseW["numberOfStages"] = wJson["number_of_stages"];
                camelCaseW["numberOfTransitions"] = wJson["number_of_transitions"];
                camelCaseW["title"] = wJson["title"];
                camelCaseW["category"] = wJson["category"];
                camelCaseW["description"] = wJson["description"];
                camelCaseW["context"] = wJson["context"];
                camelCaseW["isDefault"] = wJson["is_default"];
                camelCaseW["ordering"] = wJson["ordering"];
                camelCaseW["createdAt"] = wJson["created_at"];
                 
                data.append(camelCaseW);
            }
            response.result["data"] = data;

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.error["message"] = "Database error while fetching workflows.";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }


    drogon::Task<dto::BaseApiResponse> WorkflowService::getDetails( const std::string &id) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<Workflows>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto workflow = co_await mp.findOne(Criteria(Workflows::Cols::_id, CompareOperator::EQ, id));

            Json::Value wJson = workflow.toJson();
            Json::Value camelCaseW;
            camelCaseW["id"] = wJson["id"];
            camelCaseW["status"] = wJson["status"];
            camelCaseW["numberOfStages"] = wJson["number_of_stages"];
            camelCaseW["numberOfTransitions"] = wJson["number_of_transitions"];
            camelCaseW["title"] = wJson["title"];
            camelCaseW["category"] = wJson["category"];
            camelCaseW["description"] = wJson["description"];
            camelCaseW["context"] = wJson["context"];
            camelCaseW["isDefault"] = wJson["is_default"];
            camelCaseW["ordering"] = wJson["ordering"];
            camelCaseW["createdAt"] = wJson["created_at"];

            response.success = true;
            response.result["data"] = camelCaseW;

        } catch (const UnexpectedRows &e) {
            response.success = false;
            response.message = "Workflow not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Database error while fetching workflow details.";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::create(const dto::WorkflowDto &workflowDto) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<Workflows>(dbClient);

        dto::BaseApiResponse response;
        try {
            size_t count = co_await mp.count(Criteria(Workflows::Cols::_title, CompareOperator::EQ, workflowDto.getTitle()));
            if (count > 0) {
                response.success = false;
                response.message = "Workflow with this title already exists";
                response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
                co_return response;
            }

            Workflows workflow;

            workflow.setTitle(workflowDto.getTitle());
            workflow.setDescription(workflowDto.getDescription());
            workflow.setContext(workflowDto.getContext());
            workflow.setCategory(workflowDto.getCategory());
            workflow.setStatus(workflowDto.getStatus());
            workflow.setIsDefault(workflowDto.getIsDefault());
            workflow.setOrdering(workflowDto.getOrdering());

            workflow.setCreatedAt(trantor::Date::now());
            workflow.setModifiedAt(trantor::Date::now());

            co_await mp.insert(workflow);

            response.success = true;
            response.message = "Workflow created successfully";
            response.result["workflowId"] = workflow.getValueOfId();

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to create workflow";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::update(const dto::WorkflowDto &workflowDto, const std::string &workflowId) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<Workflows>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto workflow = co_await mp.findOne(Criteria(Workflows::Cols::_id, CompareOperator::EQ, workflowId));

            if (!workflowDto.getTitle().empty()) {

                if (workflowDto.getTitle() != workflow.getValueOfTitle()) {
                    size_t count = co_await mp.count(Criteria(Workflows::Cols::_title, CompareOperator::EQ, workflowDto.getTitle()));
                    if (count > 0) {
                        response.success = false;
                        response.message = "Workflow with this title already exists";
                        response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
                        co_return response;
                    }
                }

                workflow.setTitle(workflowDto.getTitle());
            }
            
            if (!workflowDto.getDescription().empty()) {
                workflow.setDescription(workflowDto.getDescription());
            }

            if (!workflowDto.getContext().empty()) {
                workflow.setContext(workflowDto.getContext());
            }

            workflow.setCategory(workflowDto.getCategory());

            workflow.setStatus(workflowDto.getStatus());
            workflow.setIsDefault(workflowDto.getIsDefault());
            workflow.setOrdering(workflowDto.getOrdering());

            workflow.setModifiedAt(trantor::Date::now());

            co_await mp.update(workflow);

            response.success = true;
            response.message = "Workflow updated successfully";

        } catch (const UnexpectedRows &e) {
            response.success = false;
            response.message = "Workflow not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to update workflow";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::deleteWorkflow(const std::string &workflowId) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<Workflows>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto count = co_await mp.deleteBy(Criteria(Workflows::Cols::_id, CompareOperator::EQ, workflowId));

            if (count > 0) {
                response.success = true;
                response.message = "Workflow deleted successfully";
            } else {
                response.success = false;
                response.message = "Workflow not found";
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
            }

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to delete workflow";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::getWorkflowStages(const std::string &workflowId)
    {

        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<WorkflowStages>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto stages = co_await mp.findBy(Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowId));

            std::sort(stages.begin(), stages.end(), [](const WorkflowStages &a, const WorkflowStages &b) {
                return a.getValueOfOrdering() < b.getValueOfOrdering();
            });

            Json::Value data = Json::arrayValue;
            for (const auto &stage : stages) {
                Json::Value sJson = stage.toJson();
                Json::Value camelCaseS;
                camelCaseS["id"] = sJson["id"];
                camelCaseS["ordering"] = sJson["ordering"];
                camelCaseS["workflowId"] = sJson["workflow_id"];
                camelCaseS["status"] = sJson["status"];
                camelCaseS["title"] = sJson["title"];
                camelCaseS["description"] = sJson["description"];

                Json::Value targetUsersArray;
                Json::Reader reader;
                if (sJson.isMember("target_users") && sJson["target_users"].isString()) {
                    reader.parse(sJson["target_users"].asString(), targetUsersArray);
                }
                camelCaseS["targetUsers"] = targetUsersArray.isArray() ? targetUsersArray : Json::arrayValue;


                Json::Value targetGroupsArray;

                if (sJson.isMember("target_groups") && sJson["target_groups"].isString()) {
                    reader.parse(sJson["target_groups"].asString(), targetGroupsArray);
                }
                camelCaseS["targetGroups"] = targetGroupsArray.isArray() ? targetGroupsArray : Json::arrayValue;

                camelCaseS["isDefault"] = sJson["is_default"];
                camelCaseS["createdAt"] = sJson["created_at"];
                
                data.append(camelCaseS);
            }

            response.success = true;
            response.result["data"] = data;
            response.result["totalCount"] = (Json::UInt64)stages.size();

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.error["message"] = "Database error while fetching workflow stages.";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::createWorkflowStage(const dto::WorkflowStageDto &workflowStageDto) {
        auto dbClient = drogon::app().getDbClient();
        
        dto::BaseApiResponse response;
        try {
            auto workflowMp = CoroMapper<Workflows>(dbClient);
            size_t wfExists = co_await workflowMp.count(Criteria(Workflows::Cols::_id, CompareOperator::EQ, workflowStageDto.getWorkflowId()));
            if (wfExists == 0) {
                response.success = false;
                response.message = "Workflow not found";
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                co_return response;
            }

            auto mp = CoroMapper<WorkflowStages>(dbClient);
            
            Criteria dupCriteria = Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowStageDto.getWorkflowId()) &&
                                  Criteria(WorkflowStages::Cols::_title, CompareOperator::EQ, workflowStageDto.getTitle());
            size_t count = co_await mp.count(dupCriteria);
            if (count > 0) {
                response.success = false;
                response.message = "Stage with this title already exists in the workflow";
                response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
                co_return response;
            }

            WorkflowStages stage;

            stage.setWorkflowId(workflowStageDto.getWorkflowId());
            stage.setTitle(workflowStageDto.getTitle());
            stage.setDescription(workflowStageDto.getDescription());
            stage.setStatus(workflowStageDto.getStatus());
            stage.setOrdering(workflowStageDto.getOrdering());
            stage.setIsDefault(workflowStageDto.getIsDefault());

            Json::Value targetUsersArray = Json::arrayValue;
            for (const auto& perm : workflowStageDto.getTargetUsers()) {
                targetUsersArray.append(perm);
            }
            Json::FastWriter writer;
            std::string targetUsersStr = writer.write(targetUsersArray);
            if (!targetUsersStr.empty() && targetUsersStr.back() == '\n') {
                targetUsersStr.pop_back();
            }
            stage.setTargetUsers(targetUsersStr);

            Json::Value targetGroupsArray = Json::arrayValue;
            for (const auto& perm : workflowStageDto.getTargetUsers()) {
                targetGroupsArray.append(perm);
            }

            std::string targetGroupsStr = writer.write(targetGroupsArray);
            if (!targetGroupsStr.empty() && targetGroupsStr.back() == '\n') {
                targetGroupsStr.pop_back();
            }

            stage.setTargetGroups(targetGroupsStr);

            stage.setCreatedAt(trantor::Date::now());
            stage.setModifiedAt(trantor::Date::now());

            co_await mp.insert(stage);

            response.success = true;
            response.message = "Workflow stage created successfully";
            response.result["stageId"] = stage.getValueOfId();

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to create workflow stage";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::updateWorkflowStage(const dto::WorkflowStageDto &workflowStageDto, const std::string &id) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<WorkflowStages>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto stage = co_await mp.findOne(Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, id) && 
                                             Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowStageDto.getWorkflowId()));

            if (!workflowStageDto.getTitle().empty()) {
                if (workflowStageDto.getTitle() != stage.getValueOfTitle()) {
                    Criteria dupCriteria = Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowStageDto.getWorkflowId()) &&
                                          Criteria(WorkflowStages::Cols::_title, CompareOperator::EQ, workflowStageDto.getTitle());
                    size_t count = co_await mp.count(dupCriteria);
                    if (count > 0) {
                        response.success = false;
                        response.message = "Stage with this title already exists in the workflow";
                        response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
                        co_return response;
                    }
                }
                stage.setTitle(workflowStageDto.getTitle());
            }

            if (!workflowStageDto.getDescription().empty()) {
                stage.setDescription(workflowStageDto.getDescription());
            }

            stage.setStatus(workflowStageDto.getStatus());
            stage.setOrdering(workflowStageDto.getOrdering());
            stage.setIsDefault(workflowStageDto.getIsDefault());

            stage.setModifiedAt(trantor::Date::now());

            co_await mp.update(stage);

            response.success = true;
            response.message = "Workflow stage updated successfully";

        } catch (const UnexpectedRows &e) {
            response.success = false;
            response.message = "Workflow stage not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to update workflow stage";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::deleteWorkflowStage(const std::string &workflowId, const std::string &id) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<WorkflowStages>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto count = co_await mp.deleteBy(Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, id) && 
                                              Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowId));

            if (count > 0) {
                response.success = true;
                response.message = "Workflow stage deleted successfully";
            } else {
                response.success = false;
                response.message = "Workflow stage not found";
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
            }

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to delete workflow stage";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::getWorkflowTransitions(const std::string &workflowId) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<WorkflowTransitions>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto transitions = co_await mp.findBy(Criteria(WorkflowTransitions::Cols::_workflow_id, CompareOperator::EQ, workflowId));

            std::sort(transitions.begin(), transitions.end(), [](const WorkflowTransitions &a, const WorkflowTransitions &b) {
                return a.getValueOfOrdering() < b.getValueOfOrdering();
            });

            Json::Value data = Json::arrayValue;
            for (const auto &trans : transitions) {
                Json::Value tJson = trans.toJson();
                Json::Value camelCaseT;
                camelCaseT["id"] = tJson["id"];
                camelCaseT["assetId"] = tJson["asset_id"];
                camelCaseT["ordering"] = tJson["ordering"];
                camelCaseT["workflowId"] = tJson["workflow_id"];
                camelCaseT["published"] = tJson["published"];
                camelCaseT["title"] = tJson["title"];
                camelCaseT["description"] = tJson["description"];
                camelCaseT["fromStageId"] = tJson["from_stage_id"];
                camelCaseT["toStageId"] = tJson["to_stage_id"];
                camelCaseT["options"] = tJson["options"];
                
                camelCaseT["createdAt"] = tJson["created_at"];
                
                data.append(camelCaseT);
            }

            response.success = true;
            response.result["data"] = data;
            response.result["totalCount"] = (Json::UInt64)transitions.size();

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.error["message"] = "Database error while fetching workflow transitions.";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::createWorkflowTransition(const dto::WorkflowTransitionDto &workflowTransitionDto) {
        auto dbClient = drogon::app().getDbClient();
        
        dto::BaseApiResponse response;
        try {
            auto workflowMp = CoroMapper<Workflows>(dbClient);
            size_t wfExists = co_await workflowMp.count(Criteria(Workflows::Cols::_id, CompareOperator::EQ, workflowTransitionDto.getWorkflowId()));
            if (wfExists == 0) {
                response.success = false;
                response.message = "Workflow not found";
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                co_return response;
            }

            auto stageMp = CoroMapper<WorkflowStages>(dbClient);
            size_t fromExists = co_await stageMp.count(
                Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, workflowTransitionDto.getFromStageId()) &&
                Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowTransitionDto.getWorkflowId())
            );
            size_t toExists = co_await stageMp.count(
                Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, workflowTransitionDto.getToStageId()) &&
                Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowTransitionDto.getWorkflowId())
            );
            if (fromExists == 0 || toExists == 0) {
                response.success = false;
                response.message = "From/To stage not found in this workflow";
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                co_return response;
            }

            auto mp = CoroMapper<WorkflowTransitions>(dbClient);

            if (!workflowTransitionDto.getTitle().empty()) {
                Criteria dupCriteria = Criteria(WorkflowTransitions::Cols::_workflow_id, CompareOperator::EQ, workflowTransitionDto.getWorkflowId()) &&
                                      Criteria(WorkflowTransitions::Cols::_title, CompareOperator::EQ, workflowTransitionDto.getTitle());
                size_t count = co_await mp.count(dupCriteria);
                if (count > 0) {
                    response.success = false;
                    response.message = "Transition with this title already exists in the workflow";
                    response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
                    co_return response;
                }
            }

            WorkflowTransitions transition;

            transition.setWorkflowId(workflowTransitionDto.getWorkflowId());
            transition.setTitle(workflowTransitionDto.getTitle());
            transition.setDescription(workflowTransitionDto.getDescription());
            transition.setStatus(workflowTransitionDto.getStatus());
            transition.setOrdering(workflowTransitionDto.getOrdering());
            transition.setFromStageId(workflowTransitionDto.getFromStageId());
            transition.setToStageId(workflowTransitionDto.getToStageId());
            transition.setOptions(workflowTransitionDto.getOptions());


            transition.setCreatedAt(trantor::Date::now());
            transition.setModifiedAt(trantor::Date::now());

            co_await mp.insert(transition);

            response.success = true;
            response.message = "Workflow transition created successfully";
            response.result["transitionId"] = transition.getValueOfId();

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to create workflow transition";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::updateWorkflowTransition(const dto::WorkflowTransitionDto &workflowTransitionDto, const std::string &id) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<WorkflowTransitions>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto transition = co_await mp.findOne(Criteria(WorkflowTransitions::Cols::_id, CompareOperator::EQ, id) && 
                                                  Criteria(WorkflowTransitions::Cols::_workflow_id, CompareOperator::EQ, workflowTransitionDto.getWorkflowId()));

            if (!workflowTransitionDto.getTitle().empty()) {
                if (workflowTransitionDto.getTitle() != transition.getValueOfTitle()) {
                    Criteria dupCriteria = Criteria(WorkflowTransitions::Cols::_workflow_id, CompareOperator::EQ, workflowTransitionDto.getWorkflowId()) &&
                                          Criteria(WorkflowTransitions::Cols::_title, CompareOperator::EQ, workflowTransitionDto.getTitle());
                    size_t count = co_await mp.count(dupCriteria);
                    if (count > 0) {
                        response.success = false;
                        response.message = "Transition with this title already exists in the workflow";
                        response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
                        co_return response;
                    }
                }
                transition.setTitle(workflowTransitionDto.getTitle());
            }

            auto stageMp = CoroMapper<WorkflowStages>(dbClient);
            if (!workflowTransitionDto.getFromStageId().empty() && workflowTransitionDto.getFromStageId() != transition.getValueOfFromStageId()) {
                size_t fromExists = co_await stageMp.count(
                    Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, workflowTransitionDto.getFromStageId()) &&
                    Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowTransitionDto.getWorkflowId())
                );
                if (fromExists == 0) {
                    response.success = false;
                    response.message = "From stage not found in this workflow";
                    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                    co_return response;
                }
                transition.setFromStageId(workflowTransitionDto.getFromStageId());
            }

            if (!workflowTransitionDto.getToStageId().empty() && workflowTransitionDto.getToStageId() != transition.getValueOfToStageId()) {
                size_t toExists = co_await stageMp.count(
                    Criteria(WorkflowStages::Cols::_id, CompareOperator::EQ, workflowTransitionDto.getToStageId()) &&
                    Criteria(WorkflowStages::Cols::_workflow_id, CompareOperator::EQ, workflowTransitionDto.getWorkflowId())
                );
                if (toExists == 0) {
                    response.success = false;
                    response.message = "To stage not found in this workflow";
                    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                    co_return response;
                }
                transition.setToStageId(workflowTransitionDto.getToStageId());
            }

            if (!workflowTransitionDto.getDescription().empty()) {
                transition.setDescription(workflowTransitionDto.getDescription());
            }
            if (!workflowTransitionDto.getOptions().empty()) {
                transition.setOptions(workflowTransitionDto.getOptions());
            }

            transition.setStatus(workflowTransitionDto.getStatus());
            transition.setOrdering(workflowTransitionDto.getOrdering());


            transition.setModifiedAt(trantor::Date::now());

            co_await mp.update(transition);

            response.success = true;
            response.message = "Workflow transition updated successfully";

        } catch (const UnexpectedRows &e) {
            response.success = false;
            response.message = "Workflow transition not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to update workflow transition";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> WorkflowService::deleteWorkflowTransition(const std::string &workflowId, const std::string &id) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<WorkflowTransitions>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto count = co_await mp.deleteBy(Criteria(WorkflowTransitions::Cols::_id, CompareOperator::EQ, id) && 
                                              Criteria(WorkflowTransitions::Cols::_workflow_id, CompareOperator::EQ, workflowId));

            if (count > 0) {
                response.success = true;
                response.message = "Workflow transition deleted successfully";
            } else {
                response.success = false;
                response.message = "Workflow transition not found";
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
            }

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to delete workflow transition";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

}