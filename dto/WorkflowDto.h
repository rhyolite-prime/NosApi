//
// Created by Emmanuel Addo-Odame on 19/05/2026.
//

#ifndef NOSAPI_WORKFLOWDTO_H
#define NOSAPI_WORKFLOWDTO_H

#include <json/json.h>
#include <string>
#include <optional>

namespace nos::dto {

    class WorkflowDto {

    public:
        WorkflowDto() = default;
        explicit WorkflowDto(const Json::Value& json) { fromJson(json); }

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getContext() const { return context_; }
        [[nodiscard]] const std::string& getCategory() const { return category_; }
        [[nodiscard]] int getStatus() const { return status_; }
        [[nodiscard]] bool getIsDefault() const { return isDefault_; }
        [[nodiscard]] int getOrdering() const { return ordering_; }


        // Setters
        void setTitle(const std::string& title) { title_ = title; }
        void setDescription(const std::string& description) { description_ = description; }
        void setContext(const std::string& context) { context_ = context; }
        void setCategory(const std::string& category) { category_ = category; }
        void setStatus(int status) { status_ = status; }
        void setIsDefault(bool isDefault) { isDefault_ = isDefault; }
        void setOrdering(int ordering) { ordering_ = ordering; }


    private:
        std::string title_;
        std::string description_;
        std::string context_;
        std::string category_;
        int status_ = 1;
        bool isDefault_ = false;
        int ordering_ = 0;
    };

    inline void WorkflowDto::fromJson(const Json::Value& json) {

        if (json.isMember("title") && !json["title"].isNull())
            title_ = json["title"].asString();

        if (json.isMember("description") && !json["description"].isNull())
            description_ = json["description"].asString();

        if (json.isMember("context") && !json["context"].isNull())
            context_ = json["context"].asString();

        if (json.isMember("category") && !json["category"].isNull())
            category_ = json["category"].asString();

        if (json.isMember("status") && !json["status"].isNull())
            status_ = json["status"].asInt();

        if (json.isMember("isDefault") && !json["isDefault"].isNull())
            isDefault_ = json["isDefault"].asBool();

        if (json.isMember("ordering") && !json["ordering"].isNull())
            ordering_ = json["ordering"].asInt();

    }

}

#endif //NOSAPI_WORKFLOWDTO_H