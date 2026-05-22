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
        [[nodiscard]] int getPublished() const { return published_; }
        [[nodiscard]] int getOrdering() const { return ordering_; }
        [[nodiscard]] const std::string& getFromStageId() const { return fromStageId_; }
        [[nodiscard]] const std::string& getToStageId() const { return toStageId_; }
        [[nodiscard]] const std::string& getOptions() const { return options_; }
        [[nodiscard]] const std::string& getCreatedBy() const { return createdBy_; }
        [[nodiscard]] const std::string& getModifiedBy() const { return modifiedBy_; }

        // Setters
        void setWorkflowId(const std::string& workflowId) { workflowId_ = workflowId; }
        void setTitle(const std::string& title) { title_ = title; }
        void setDescription(const std::string& description) { description_ = description; }
        void setPublished(int published) { published_ = published; }
        void setOrdering(int ordering) { ordering_ = ordering; }
        void setFromStageId(const std::string& fromStageId) { fromStageId_ = fromStageId; }
        void setToStageId(const std::string& toStageId) { toStageId_ = toStageId; }
        void setOptions(const std::string& options) { options_ = options; }
        void setCreatedBy(const std::string& createdBy) { createdBy_ = createdBy; }
        void setModifiedBy(const std::string& modifiedBy) { modifiedBy_ = modifiedBy; }

    private:
        std::string workflowId_;
        std::string title_;
        std::string description_;
        int published_ = 1;
        int ordering_ = 0;
        std::string fromStageId_;
        std::string toStageId_;
        std::string options_;
        std::string createdBy_;
        std::string modifiedBy_;
    };

    inline void WorkflowTransitionDto::fromJson(const Json::Value& json) {
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

        if (json.isMember("fromStageId") && !json["fromStageId"].isNull())
            fromStageId_ = json["fromStageId"].asString();

        if (json.isMember("toStageId") && !json["toStageId"].isNull())
            toStageId_ = json["toStageId"].asString();

        if (json.isMember("options") && !json["options"].isNull())
            options_ = json["options"].asString();

        if (json.isMember("createdBy") && !json["createdBy"].isNull())
            createdBy_ = json["createdBy"].asString();

        if (json.isMember("modifiedBy") && !json["modifiedBy"].isNull())
            modifiedBy_ = json["modifiedBy"].asString();
    }

}


#endif //NOSAPI_WORKFLOWTRANSITIONDTO_H