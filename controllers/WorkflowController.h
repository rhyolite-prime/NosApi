#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class WorkflowController : public drogon::HttpController<WorkflowController>
{
  public:
  static constexpr const char *PREFIX = "/api/v1/work-flow/";
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(WorkflowController::getAll, std::string(PREFIX) + "get-all", Get, Options);
  ADD_METHOD_TO(WorkflowController::createWorkflow, std::string(PREFIX) + "create", Post, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::updateWorkflow,std::string(PREFIX) + "update/{1}", Put, Options, "JwtAuthFilter");
  ADD_METHOD_TO(WorkflowController::deleteWorkflow, std::string(PREFIX) + "delete/{1}", Delete, Options, "JwtAuthFilter");
  METHOD_LIST_END

  Task<HttpResponsePtr> getAll(HttpRequestPtr req);
  Task<HttpResponsePtr> createWorkflow(HttpRequestPtr req);
  Task<HttpResponsePtr> updateWorkflow(HttpRequestPtr req, std::string workflowId);
  Task<HttpResponsePtr> deleteWorkflow(HttpRequestPtr req, std::string workflowId);

};
