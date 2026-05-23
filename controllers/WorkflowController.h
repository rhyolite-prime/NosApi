#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class WorkflowController : public drogon::HttpController<WorkflowController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/work-flow/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(WorkflowController::getAll, std::string(PREFIX) + "get-all", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::getWorkflowDetails, std::string(PREFIX) + "{1}", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::createWorkflow, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::updateWorkflow,std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::deleteWorkflow, std::string(PREFIX) + "delete/{1}", Delete, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::getStages, std::string(PREFIX) + "get-stages/{1}", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::createStage, std::string(PREFIX) + "create-stage", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::updateStage,std::string(PREFIX) + "update-stage/{1}", Put, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::deleteStage, std::string(PREFIX) + "delete-stage/{1}", Delete, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::getTransitions, std::string(PREFIX) + "get-transitions/{1}", Get, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::createTransition, std::string(PREFIX) + "create-transition", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::updateTransition,std::string(PREFIX) + "update-transition/{1}", Put, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::deleteTransition, std::string(PREFIX) + "delete-transition/{1}", Delete, Options, "JwtAuthFilter");
  METHOD_LIST_END

  Task<HttpResponsePtr> getAll(HttpRequestPtr req);
  Task<HttpResponsePtr> getWorkflowDetails(HttpRequestPtr req, std::string id);
  Task<HttpResponsePtr> createWorkflow(HttpRequestPtr req);
  Task<HttpResponsePtr> updateWorkflow(HttpRequestPtr req, std::string workflowId);
  Task<HttpResponsePtr> deleteWorkflow(HttpRequestPtr req, std::string workflowId);

  //stages
  Task<HttpResponsePtr> getStages(HttpRequestPtr req, std::string workflowId);
  Task<HttpResponsePtr> createStage(HttpRequestPtr req);
  Task<HttpResponsePtr> updateStage(HttpRequestPtr req, const std::string &id);
  Task<HttpResponsePtr> deleteStage(HttpRequestPtr req, const std::string &id);

  //transition
  Task<HttpResponsePtr> getTransitions(HttpRequestPtr req, std::string workflowId);
  Task<HttpResponsePtr> createTransition(HttpRequestPtr req);
  Task<HttpResponsePtr> updateTransition(HttpRequestPtr req, const std::string &id);
  Task<HttpResponsePtr> deleteTransition(HttpRequestPtr req, const std::string &id);

};
