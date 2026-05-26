//
// Created by Emmanuel Addo-Odame on 19/05/2026.
//

#ifndef NOSAPI_WORKFLOWSTAGEDTO_H
#define NOSAPI_WORKFLOWSTAGEDTO_H

#include <json/json.h>

namespace nos::dto {

    class WorkflowStageDto {

    public:
        WorkflowStageDto() = default;
        explicit WorkflowStageDto(const Json::Value& json) { fromJson(json); }

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getWorkflowId() const { return workflowId_; }
        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] int getStatus() const { return status_; }
        [[nodiscard]] int getOrdering() const { return ordering_; }
        [[nodiscard]] bool getIsDefault() const { return isDefault_; }
        [[nodiscard]] const std::vector<std::string> &getTargetUsers() const { return target_users_; }
        [[nodiscard]] const std::vector<std::string> &getTargetGroups() const { return target_groups_; }


        // Setters
        void setWorkflowId(const std::string& workflowId) { workflowId_ = workflowId; }
        void setTitle(const std::string& title) { title_ = title; }
        void setDescription(const std::string& description) { description_ = description; }
        void setStatus(int status) { status_ = status; }
        void setOrdering(int ordering) { ordering_ = ordering; }
        void setIsDefault(bool isDefault) { isDefault_ = isDefault; }
        void setTargetUsers(const std::vector<std::string> &value) { target_users_ = value; }
        void setTargetGroups(const std::vector<std::string> &value) { target_groups_ = value; }


    private:
        std::string workflowId_;
        std::string title_;
        std::string description_;
        int status_ = 0;
        int ordering_ = 0;
        bool isDefault_ = false;
        std::vector<std::string> target_users_;
        std::vector<std::string> target_groups_;

    };

    inline void WorkflowStageDto::fromJson(const Json::Value& json) {

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

        if (json.isMember("isDefault") && !json["isDefault"].isNull())
            isDefault_ = json["isDefault"].asBool();

        if (json.isMember("targetUsers") && json["targetUsers"].isArray()) {
            for (const auto &ip : json["targetUsers"]) {
                target_users_.push_back(ip.asString());
            }
        }

        if (json.isMember("targetGroups") && json["targetGroups"].isArray()) {
            for (const auto &ip : json["targetGroups"]) {
                target_groups_.push_back(ip.asString());
            }
        }

    }

}



#endif //NOSAPI_WORKFLOWSTAGEDTO_H