//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#ifndef NOSAPI_WORKFLOWSERVICE_H
#define NOSAPI_WORKFLOWSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/WorkflowDto.h"
#include "dto/WorkflowStageDto.h"
#include "dto/WorkflowTransitionDto.h"
#include <drogon/drogon.h>

namespace nos::services {

class WorkflowService {

public:
  drogon::Task<dto::BaseApiResponse> getAll(int pageNo, int pageSize, const std::string &query);
  drogon::Task<dto::BaseApiResponse> getDetails(const std::string &id);
  drogon::Task<dto::BaseApiResponse> create(const dto::WorkflowDto &workflowDto);
  drogon::Task<dto::BaseApiResponse> update(const dto::WorkflowDto &workflowDto, const std::string &workflowId);
  drogon::Task<dto::BaseApiResponse> deleteWorkflow(const std::string &workflowId);

  // stages
  drogon::Task<dto::BaseApiResponse> getWorkflowStages(const std::string &workflowId);
  drogon::Task<dto::BaseApiResponse> createWorkflowStage(const dto::WorkflowStageDto &workflowStageDto);
  drogon::Task<dto::BaseApiResponse> updateWorkflowStage(const dto::WorkflowStageDto &workflowStageDto, const std::string &id);
  drogon::Task<dto::BaseApiResponse>  deleteWorkflowStage(const std::string &workflowId, const std::string &id);

  // transitions
  drogon::Task<dto::BaseApiResponse> getWorkflowTransitions(const std::string &workflowId);
  drogon::Task<dto::BaseApiResponse> createWorkflowTransition(const dto::WorkflowTransitionDto &workflowTransitionDto);
  drogon::Task<dto::BaseApiResponse> updateWorkflowTransition(const dto::WorkflowTransitionDto &workflowTransitionDto, const std::string &id);
  drogon::Task<dto::BaseApiResponse> deleteWorkflowTransition(const std::string &workflowId, const std::string &id);

  // handover
  drogon::Task<void> triggerWorkflow(const std::string &entityId, const std::string &workflowId, const std::string &stageId);
};

} // namespace nos::services
#endif // NOSAPI_WORKFLOWSERVICE_H