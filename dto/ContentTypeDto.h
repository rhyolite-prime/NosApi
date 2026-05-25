//
// Created by Emmanuel Addo-Odame on 25/05/2026.
//

#ifndef NOSAPI_CONTENTTYPEDTO_H
#define NOSAPI_CONTENTTYPEDTO_H

#include <json/json.h>
#include <string>

namespace nos::dto {

    class ContentTypeDto {

    public:
        ContentTypeDto() = default;
        explicit ContentTypeDto(const Json::Value& json) { fromJson(json); }

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getIconClass() const { return iconClass_; }
        [[nodiscard]] const std::string& getAllowedExtensions() const { return allowedExtensions_; }
        [[nodiscard]] int64_t getMaxFileSize() const { return maxFileSize_; }
        [[nodiscard]] bool getRequiresMedia() const { return requiresMedia_; }
        [[nodiscard]] bool getRequiresThumbnail() const { return requiresThumbnail_; }
        [[nodiscard]] int32_t getSortOrder() const { return sortOrder_; }
        [[nodiscard]] bool getIsActive() const { return isActive_; }
        [[nodiscard]] bool getIsSystem() const { return isSystem_; }
        [[nodiscard]] const std::string& getMetadataSchema() const { return metadataSchema_; }

        // Setters
        void setName(const std::string& name) { name_ = name; }
        void setDescription(const std::string& description) { description_ = description; }
        void setIconClass(const std::string& iconClass) { iconClass_ = iconClass; }
        void setAllowedExtensions(const std::string& allowedExtensions) { allowedExtensions_ = allowedExtensions; }
        void setMaxFileSize(int64_t maxFileSize) { maxFileSize_ = maxFileSize; }
        void setRequiresMedia(bool requiresMedia) { requiresMedia_ = requiresMedia; }
        void setRequiresThumbnail(bool requiresThumbnail) { requiresThumbnail_ = requiresThumbnail; }
        void setSortOrder(int32_t sortOrder) { sortOrder_ = sortOrder; }
        void setIsActive(bool isActive) { isActive_ = isActive; }
        void setIsSystem(bool isSystem) { isSystem_ = isSystem; }
        void setMetadataSchema(const std::string& metadataSchema) { metadataSchema_ = metadataSchema; }

    private:
        std::string name_;
        std::string description_;
        std::string iconClass_;
        std::string allowedExtensions_;
        int64_t maxFileSize_ = 0;
        bool requiresMedia_ = false;
        bool requiresThumbnail_ = false;
        int32_t sortOrder_ = 0;
        bool isActive_ = true;
        bool isSystem_ = false;
        std::string metadataSchema_;
    };

    inline void ContentTypeDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }
        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }
        if (json.isMember("iconClass") && !json["iconClass"].isNull()) {
            iconClass_ = json["iconClass"].asString();
        }
        if (json.isMember("allowedExtensions") && !json["allowedExtensions"].isNull()) {
            allowedExtensions_ = json["allowedExtensions"].asString();
        }
        if (json.isMember("maxFileSize") && !json["maxFileSize"].isNull()) {
            maxFileSize_ = json["maxFileSize"].asInt64();
        }
        if (json.isMember("requiresMedia") && !json["requiresMedia"].isNull()) {
            requiresMedia_ = json["requiresMedia"].asBool();
        }
        if (json.isMember("requiresThumbnail") && !json["requiresThumbnail"].isNull()) {
            requiresThumbnail_ = json["requiresThumbnail"].asBool();
        }
        if (json.isMember("sortOrder") && !json["sortOrder"].isNull()) {
            sortOrder_ = json["sortOrder"].asInt();
        }
        if (json.isMember("isActive") && !json["isActive"].isNull()) {
            isActive_ = json["isActive"].asBool();
        }
        if (json.isMember("isSystem") && !json["isSystem"].isNull()) {
            isSystem_ = json["isSystem"].asBool();
        }
        if (json.isMember("metadataSchema") && !json["metadataSchema"].isNull()) {
            metadataSchema_ = json["metadataSchema"].asString();
        }
    }

}

#endif //NOSAPI_CONTENTTYPEDTO_H