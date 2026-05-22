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
        [[nodiscard]] int getPublished() const { return published_; }
        [[nodiscard]] int getOrdering() const { return ordering_; }
        [[nodiscard]] bool getIsDefault() const { return isDefault_; }
        [[nodiscard]] const std::string& getCreatedBy() const { return createdBy_; }
        [[nodiscard]] const std::string& getModifiedBy() const { return modifiedBy_; }

        // Setters
        void setWorkflowId(const std::string& workflowId) { workflowId_ = workflowId; }
        void setTitle(const std::string& title) { title_ = title; }
        void setDescription(const std::string& description) { description_ = description; }
        void setPublished(int published) { published_ = published; }
        void setOrdering(int ordering) { ordering_ = ordering; }
        void setIsDefault(bool isDefault) { isDefault_ = isDefault; }
        void setCreatedBy(const std::string& createdBy) { createdBy_ = createdBy; }
        void setModifiedBy(const std::string& modifiedBy) { modifiedBy_ = modifiedBy; }

    private:
        std::string workflowId_;
        std::string title_;
        std::string description_;
        int published_ = 1;
        int ordering_ = 0;
        bool isDefault_ = false;
        std::string createdBy_;
        std::string modifiedBy_;
    };

    inline void WorkflowStageDto::fromJson(const Json::Value& json) {
        if (json.isMember("workflowId") && !json["workflowId"].isNull())
            workflowId_ = json["workflowId"].asString();

        if (json.isMember("title") && !json["title"].isNull())
            title_ = json["title"].asString();

        if (json.isMember("description") && !json["description"].isNull())
            description_ = json["description"].asString();

        if (json.isMember("published") && !json["published"].isNull())
            published_ = json["published"].asInt();

        if (json.isMember("ordering") && !json["ordering"].isNull())
            ordering_ = json["ordering"].asInt();

        if (json.isMember("isDefault") && !json["isDefault"].isNull())
            isDefault_ = json["isDefault"].asBool();

        if (json.isMember("createdBy") && !json["createdBy"].isNull())
            createdBy_ = json["createdBy"].asString();

        if (json.isMember("modifiedBy") && !json["modifiedBy"].isNull())
            modifiedBy_ = json["modifiedBy"].asString();
    }

}



#endif //NOSAPI_WORKFLOWSTAGEDTO_H