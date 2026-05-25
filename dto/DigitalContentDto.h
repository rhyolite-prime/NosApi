//
// Created by Emmanuel Addo-Odame on 24/05/2026.
//

#ifndef NOSAPI_DIGITALCONTENTDTO_H
#define NOSAPI_DIGITALCONTENTDTO_H
#include <json/json.h>
#include <string>

namespace nos::dto {

    class DigitalContentDto {
    public:
        DigitalContentDto() = default;
        explicit DigitalContentDto(const Json::Value& json) { fromJson(json); }

        void fromJson(const Json::Value& json);

        // Getters

        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] const std::string& getAlias() const { return alias_; }
        [[nodiscard]] const std::string& getContentTypeId() const { return contentTypeId_; }
        [[nodiscard]] const std::string& getContentTypeName() const { return contentType_; }
        [[nodiscard]] const std::string& getIntrotext() const { return introtext_; }
        [[nodiscard]] const std::string& getFulltext() const { return fulltext_; }

        [[nodiscard]] int32_t getStatus() const { return status_; }
        [[nodiscard]] const std::string& getCategoryId() const { return categoryId_; }
        [[nodiscard]] int32_t getOrdering() const { return ordering_; }
        [[nodiscard]] int16_t getFeatured() const { return featured_; }
        [[nodiscard]] const std::string& getAccessLevelId() const { return accessLevelId_; }
        [[nodiscard]] const std::string& getMetakey() const { return metakey_; }
        [[nodiscard]] const std::string& getMetadesc() const { return metadesc_; }
        [[nodiscard]] const Json::Value& getMetadata() const { return metadata_; }
        [[nodiscard]] const Json::Value& getPresentationConfig() const { return presentationConfig_; }
        [[nodiscard]] const Json::Value& getDisplayOverrides() const { return displayOverrides_; }

        [[nodiscard]] int32_t getVersion() const { return version_; }
        [[nodiscard]] const std::string& getLanguage() const { return language_; }
        [[nodiscard]] const std::string& getCreatedBy() const { return createdBy_; }
        [[nodiscard]] const std::string& getModifiedBy() const { return modifiedBy_; }
        [[nodiscard]] int32_t getViews() const { return views_; }



        // Setters

        void setTitle(const std::string& title) { title_ = title; has_title_ = true; }
        void setAlias(const std::string& alias) { alias_ = alias; has_alias_ = true; }
        void setContentTypeId(const std::string& val) { contentTypeId_ = val; has_content_type_id_ = true; }
        void setContentType(const std::string& val) { contentType_ = val; has_content_type_ = true; }
        void setIntrotext(const std::string& val) { introtext_ = val; has_introtext_ = true; }
        void setFulltext(const std::string& val) { fulltext_ = val; has_fulltext_ = true; }

        void setStatus(int32_t val) { status_ = val; has_status_ = true; }
        void setCategoryId(const std::string& val) { categoryId_ = val; has_category_id_ = true; }
        void setOrdering(int32_t val) { ordering_ = val; has_ordering_ = true; }
        void setFeatured(int16_t val) { featured_ = val; has_featured_ = true; }
        void setAccessLevelId(const std::string& val) { accessLevelId_ = val; has_access_level_id_ = true; }
        void setMetakey(const std::string& val) { metakey_ = val; has_metakey_ = true; }
        void setMetadesc(const std::string& val) { metadesc_ = val; has_metadesc_ = true; }
        void setMetadata(const Json::Value& val) { metadata_ = val; has_metadata_ = true; }
        void setPresentationConfig(const Json::Value& val) { presentationConfig_ = val; has_presentation_config_ = true; }
        void setDisplayOverrides(const Json::Value& val) { displayOverrides_ = val; has_display_overrides_ = true; }

        void setVersion(int32_t val) { version_ = val; has_version_ = true; }
        void setLanguage(const std::string& val) { language_ = val; has_language_ = true; }
        void setCreatedBy(const std::string& val) { createdBy_ = val; has_created_by_ = true; }
        void setModifiedBy(const std::string& val) { modifiedBy_ = val; has_modified_by_ = true; }
        void setViews(int32_t val) { views_ = val; has_views_ = true; }

    private:

        std::string title_;
        std::string alias_;
        std::string contentTypeId_;
        std::string contentType_;
        std::string introtext_;
        std::string fulltext_;
        std::string primaryResourceId_;
        int32_t status_ = 0;
        std::string categoryId_;
        int32_t ordering_ = 0;
        int16_t featured_ = 0;
        std::string accessLevelId_;
        std::string metakey_;
        std::string metadesc_;
        Json::Value metadata_ = Json::objectValue;
        Json::Value presentationConfig_ = Json::objectValue;
        Json::Value displayOverrides_ = Json::objectValue;

        int32_t version_ = 1;
        std::string language_ = "*";
        std::string createdBy_;
        std::string modifiedBy_;
        int32_t views_ = 0;

        bool has_id_ = false;
        bool has_title_ = false;
        bool has_alias_ = false;
        bool has_content_type_id_ = false;
        bool has_content_type_ = false;
        bool has_introtext_ = false;
        bool has_fulltext_ = false;
        bool has_primary_resource_id_ = false;
        bool has_status_ = false;
        bool has_category_id_ = false;
        bool has_ordering_ = false;
        bool has_featured_ = false;
        bool has_access_level_id_ = false;
        bool has_metakey_ = false;
        bool has_metadesc_ = false;
        bool has_metadata_ = false;
        bool has_presentation_config_ = false;
        bool has_display_overrides_ = false;

        bool has_version_ = false;
        bool has_language_ = false;
        bool has_created_by_ = false;
        bool has_modified_by_ = false;
        bool has_views_ = false;
    };

    inline void DigitalContentDto::fromJson(const Json::Value& json) {

        if (json.isMember("title")) {
            has_title_ = true;
            if (!json["title"].isNull()) title_ = json["title"].asString();
        }
        if (json.isMember("alias")) {
            has_alias_ = true;
            if (!json["alias"].isNull()) alias_ = json["alias"].asString();
        }
        if (json.isMember("contentTypeId")) {
            has_content_type_id_ = true;
            if (!json["contentTypeId"].isNull()) contentTypeId_ = json["contentTypeId"].asString();
        }
        if (json.isMember("contentType")) {
            has_content_type_ = true;
            if (!json["contentType"].isNull()) contentType_ = json["contentType"].asString();
        }
        if (json.isMember("introtext")) {
            has_introtext_ = true;
            if (!json["introtext"].isNull()) introtext_ = json["introtext"].asString();
        }
        if (json.isMember("fulltext")) {
            has_fulltext_ = true;
            if (!json["fulltext"].isNull()) fulltext_ = json["fulltext"].asString();
        }
        if (json.isMember("primaryResourceId")) {
            has_primary_resource_id_ = true;
            if (!json["primaryResourceId"].isNull()) primaryResourceId_ = json["primaryResourceId"].asString();
        }
        if (json.isMember("status")) {
            has_status_ = true;
            if (!json["status"].isNull()) status_ = json["status"].asInt();
        }
        if (json.isMember("categoryId")) {
            has_category_id_ = true;
            if (!json["categoryId"].isNull()) categoryId_ = json["categoryId"].asString();
        }
        if (json.isMember("ordering")) {
            has_ordering_ = true;
            if (!json["ordering"].isNull()) ordering_ = json["ordering"].asInt();
        }
        if (json.isMember("featured")) {
            has_featured_ = true;
            if (!json["featured"].isNull()) featured_ = (int16_t)json["featured"].asInt();
        }
        if (json.isMember("accessLevelId")) {
            has_access_level_id_ = true;
            if (!json["accessLevelId"].isNull()) accessLevelId_ = json["accessLevelId"].asString();
        }
        if (json.isMember("metakey")) {
            has_metakey_ = true;
            if (!json["metakey"].isNull()) metakey_ = json["metakey"].asString();
        }
        if (json.isMember("metadesc")) {
            has_metadesc_ = true;
            if (!json["metadesc"].isNull()) metadesc_ = json["metadesc"].asString();
        }
        if (json.isMember("metadata")) {
            has_metadata_ = true;
            if (!json["metadata"].isNull()) metadata_ = json["metadata"];
        }
        if (json.isMember("presentationConfig")) {
            has_presentation_config_ = true;
            if (!json["presentationConfig"].isNull()) presentationConfig_ = json["presentationConfig"];
        }
        if (json.isMember("displayOverrides")) {
            has_display_overrides_ = true;
            if (!json["displayOverrides"].isNull()) displayOverrides_ = json["displayOverrides"];
        }

        if (json.isMember("version")) {
            has_version_ = true;
            if (!json["version"].isNull()) version_ = json["version"].asInt();
        }
        if (json.isMember("language")) {
            has_language_ = true;
            if (!json["language"].isNull()) language_ = json["language"].asString();
        }

        if (json.isMember("views")) {
            has_views_ = true;
            if (!json["views"].isNull()) views_ = json["views"].asInt();
        }
    }

}
#endif //NOSAPI_DIGITALCONTENTDTO_H