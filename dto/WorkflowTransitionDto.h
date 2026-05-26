//
// Created by Emmanuel Addo-Odame on 19/05/2026.
//

#ifndef NOSAPI_WORKFLOWTRANSITIONDTO_H
#define NOSAPI_WORKFLOWTRANSITIONDTO_H

#include <json/json.h>

namespace nos::dto {

    class WorkflowTransitionDto {

    public:
        WorkflowTransitionDto() = default;
        explicit WorkflowTransitionDto(const Json::Value& json) { fromJson(json); }

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getWorkflowId() const { return workflowId_; }
        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] int getStatus() const { return status_; }
        [[nodiscard]] int getOrdering() const { return ordering_; }
        [[nodiscard]] const std::string& getCurrentStageId() const { return current_stage_id_; }
        [[nodiscard]] const std::string& getTargetStageId() const { return target_stage_id_; }
        [[nodiscard]] const std::string& getOptions() const { return options_; }


        // Setters
        void setWorkflowId(const std::string& workflowId) { workflowId_ = workflowId; }
        void setTitle(const std::string& title) { title_ = title; }
        void setDescription(const std::string& description) { description_ = description; }
        void setStatus(int status) { status_ = status; }
        void setOrdering(int ordering) { ordering_ = ordering; }
        void setFromStageId(const std::string& current_stage_id) { current_stage_id_ = current_stage_id; }
        void setToStageId(const std::string& current_stage_id) { target_stage_id_ = current_stage_id; }
        void setOptions(const std::string& options) { options_ = options; }


    private:
        std::string workflowId_;
        std::string title_;
        std::string description_;
        int status_ = 1;
        int ordering_ = 0;
        std::string current_stage_id_;
        std::string target_stage_id_;
        std::string options_;

    };

    inline void WorkflowTransitionDto::fromJson(const Json::Value& json) {

        if (json.isMember("workflowId") && !json["workflowId"].isNull())
            workflowId_ = json["workflowId"].asString();

        if (json.isMember("title") && !json["title"].isNull())
            title_ = json["title"].asString();

        if (json.isMember("description") && !json["description"].isNull())
            description_ = json["description"].asString();

        if (json.isMember("status") && !json["status"].isNull())
            status_ = json["status"].asInt();

        if (json.isMember("ordering") && !json["ordering"].isNull())
            ordering_ = json["ordering"].asInt();

        if (json.isMember("currentStageId") && !json["currentStageId"].isNull())
            current_stage_id_ = json["currentStageId"].asString();

        if (json.isMember("targetStageId") && !json["targetStageId"].isNull())
            target_stage_id_ = json["targetStageId"].asString();



    }

}


#endif //NOSAPI_WORKFLOWTRANSITIONDTO_H