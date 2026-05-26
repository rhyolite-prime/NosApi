//
// Created by Emmanuel Addo-Odame on 25/05/2026.
//

#ifndef NOSAPI_CATEGORYDTO_H
#define NOSAPI_CATEGORYDTO_H

#include <json/json.h>
#include <string>

namespace nos::dto {

    class CategoryDto {

    public:
        CategoryDto() = default;
        explicit CategoryDto(const Json::Value& json) { fromJson(json); }

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getName() const { return name_; }
        [[nodiscard]] const std::string& getAlias() const { return alias_; }
        [[nodiscard]] const std::string& getNotes() const { return notes_; }
        [[nodiscard]] const std::string& getDescription() const { return description_; }
        [[nodiscard]] const std::string& getParentId() const { return parentId_; }

        [[nodiscard]] int getStatus() const { return status_; }
        [[nodiscard]] const std::string& getAccessLevelId() const { return accessLevelId_; }
        [[nodiscard]] const std::string& getMetadesc() const { return metadesc_; }
        [[nodiscard]] const std::string& getMetakey() const { return metakey_; }
        [[nodiscard]] const std::string& getMetadata() const { return metadata_; }

        // Setters
        void setName(const std::string& name) { name_ = name; }
        void setAlias(const std::string& alias) { alias_ = alias; }
        void setNotes(const std::string& notes) { notes_ = notes; }
        void setDescription(const std::string& description) { description_ = description; }
        void setParentId(const std::string& parentId) { parentId_ = parentId; }

        void setStatus(int status) { status_ = status; }
        void setAccessLevelId(const std::string& accessLevelId) { accessLevelId_ = accessLevelId; }
        void setMetadesc(const std::string& metadesc) { metadesc_ = metadesc; }
        void setMetakey(const std::string& metakey) { metakey_ = metakey; }
        void setMetadata(const std::string& metadata) { metadata_ = metadata; }

    private:
        std::string name_;
        std::string alias_;
        std::string notes_;
        std::string description_;
        std::string parentId_;
        int status_ = 0;
        std::string accessLevelId_;
        std::string metadesc_;
        std::string metakey_;
        std::string metadata_;
    };

    inline void CategoryDto::fromJson(const Json::Value& json) {

        if (json.isMember("name") && !json["name"].isNull()) {
            name_ = json["name"].asString();
        }
        if (json.isMember("alias") && !json["alias"].isNull()) {
            alias_ = json["alias"].asString();
        }
        if (json.isMember("notes") && !json["notes"].isNull()) {
            notes_ = json["notes"].asString();
        }
        if (json.isMember("description") && !json["description"].isNull()) {
            description_ = json["description"].asString();
        }

        if (json.isMember("parentId") && !json["parentId"].isNull()) {
            parentId_ = json["parentId"].asString();
        }

        if (json.isMember("status") && !json["status"].isNull()) {
            status_ = json["status"].asBool();
        }
        if (json.isMember("accessLevelId") && !json["accessLevelId"].isNull()) {
            accessLevelId_ = json["accessLevelId"].asString();
        }

        if (json.isMember("metadesc") && !json["metadesc"].isNull()) {
            metadesc_ = json["metadesc"].asString();
        }

        if (json.isMember("metakey") && !json["metakey"].isNull()) {
            metakey_ = json["metakey"].asString();
        }

        if (json.isMember("metadata") && !json["metadata"].isNull()) {
            metadata_ = json["metadata"].asString();
        }
    }

}

#endif //NOSAPI_CATEGORYDTO_H