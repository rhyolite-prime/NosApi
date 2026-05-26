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
        [[nodiscard]] const std::string& getContentTypeName() const { return content_type_name_; }
        [[nodiscard]] const std::string& getIntrotext() const { return introtext_; }
        [[nodiscard]] const std::string& getFulltext() const { return fulltext_; }
        [[nodiscard]] int32_t getStatus() const { return status_; }
        [[nodiscard]] const std::string& getCategoryId() const { return category_id_; }
        [[nodiscard]] const std::string& getCategoryName() const { return category_name_; }
        [[nodiscard]] int32_t getOrdering() const { return ordering_; }
        [[nodiscard]] int16_t getFeatured() const { return featured_; }
        [[nodiscard]] const std::string& getAccessLevelId() const { return access_level_id_; }
        [[nodiscard]] const std::string& getAccessLevelName() const { return access_level_name_; }

        [[nodiscard]] const Json::Value& getMetadata() const { return metadata_; }
        [[nodiscard]] const Json::Value& getPresentationConfig() const { return presentationConfig_; }
        [[nodiscard]] const Json::Value& getDisplayOverrides() const { return displayOverrides_; }
        [[nodiscard]] int32_t getVersion() const { return version_; }
        [[nodiscard]] const std::string& getLanguage() const { return language_; }
        [[nodiscard]] int32_t getViews() const { return views_; }



        // Setters
        void setTitle(const std::string& title) { title_ = title;}
        void setAlias(const std::string& alias) { alias_ = alias;  }
        void setContentTypeId(const std::string& val) { contentTypeId_ = val; }
        void setContentTypeName(const std::string& val) { content_type_name_ = val; }
        void setIntrotext(const std::string& val) { introtext_ = val; }
        void setFulltext(const std::string& val) { fulltext_ = val; }

        void setStatus(int32_t val) { status_ = val; }
        void setCategoryId(const std::string& val) { category_id_ = val; }
        void setCategoryName(const std::string& val) { category_name_ = val; }
        void setOrdering(int32_t val) { ordering_ = val; }
        void setFeatured(int16_t val) { featured_ = val; }
        void setAccessLevelId(const std::string& val) { access_level_id_ = val; }
        void setAccessLevelName(const std::string& val) { access_level_name_ = val; }

        void setMetadata(const Json::Value& val) { metadata_ = val; }
        void setPresentationConfig(const Json::Value& val) { presentationConfig_ = val; }
        void setDisplayOverrides(const Json::Value& val) { displayOverrides_ = val;}

        void setVersion(int32_t val) { version_ = val; }
        void setLanguage(const std::string& val) { language_ = val; }
        void setViews(int32_t val) { views_ = val; }

    private:

        std::string title_;
        std::string alias_;
        std::string contentTypeId_;
        std::string content_type_name_;
        std::string introtext_;
        std::string fulltext_;

        int32_t status_ = 0;
        std::string category_id_;
        std::string category_name_;
        int32_t ordering_ = 0;
        int16_t featured_ = 0;
        std::string access_level_id_;
        std::string access_level_name_;
        std::string metakey_;
        std::string metadesc_;
        Json::Value metadata_ = Json::objectValue;
        Json::Value presentationConfig_ = Json::objectValue;
        Json::Value displayOverrides_ = Json::objectValue;

        int32_t version_ = 1;
        std::string language_ = "*";

        int32_t views_ = 0;

    };

    inline void DigitalContentDto::fromJson(const Json::Value& json) {

        if (json.isMember("title")) {

            if (!json["title"].isNull()) title_ = json["title"].asString();
        }
        if (json.isMember("alias")) {

            if (!json["alias"].isNull()) alias_ = json["alias"].asString();
        }
        if (json.isMember("contentTypeId")) {

            if (!json["contentTypeId"].isNull()) contentTypeId_ = json["contentTypeId"].asString();
        }
        if (json.isMember("contentTypeName")) {

            if (!json["contentTypeName"].isNull()) content_type_name_ = json["contentTypeName"].asString();
        }
        if (json.isMember("introtext")) {

            if (!json["introtext"].isNull()) introtext_ = json["introtext"].asString();
        }
        if (json.isMember("fulltext")) {

            if (!json["fulltext"].isNull()) fulltext_ = json["fulltext"].asString();
        }

        if (json.isMember("status")) {

            if (!json["status"].isNull()) status_ = json["status"].asInt();
        }
        if (json.isMember("categoryId")) {

            if (!json["categoryId"].isNull()) category_id_ = json["categoryId"].asString();
        }
        if (json.isMember("categoryName")) {

            if (!json["categoryName"].isNull()) category_name_ = json["categoryName"].asString();
        }
        if (json.isMember("ordering")) {

            if (!json["ordering"].isNull()) ordering_ = json["ordering"].asInt();
        }
        if (json.isMember("featured")) {

            if (!json["featured"].isNull()) featured_ = (int16_t)json["featured"].asInt();
        }
        if (json.isMember("accessLevelId")) {

            if (!json["accessLevelId"].isNull()) access_level_id_ = json["accessLevelId"].asString();
        }
        if (json.isMember("accessLevelName")) {

            if (!json["accessLevelName"].isNull()) access_level_name_ = json["accessLevelName"].asString();
        }
        if (json.isMember("metakey")) {

            if (!json["metakey"].isNull()) metakey_ = json["metakey"].asString();
        }
        if (json.isMember("metadesc")) {

            if (!json["metadesc"].isNull()) metadesc_ = json["metadesc"].asString();
        }
        if (json.isMember("metadata")) {

            if (!json["metadata"].isNull()) metadata_ = json["metadata"];
        }
        if (json.isMember("presentationConfig")) {

            if (!json["presentationConfig"].isNull()) presentationConfig_ = json["presentationConfig"];
        }
        if (json.isMember("displayOverrides")) {

            if (!json["displayOverrides"].isNull()) displayOverrides_ = json["displayOverrides"];
        }

        if (json.isMember("version")) {

            if (!json["version"].isNull()) version_ = json["version"].asInt();
        }
        if (json.isMember("language")) {

            if (!json["language"].isNull()) language_ = json["language"].asString();
        }

        if (json.isMember("views")) {

            if (!json["views"].isNull()) views_ = json["views"].asInt();
        }
    }

}
#endif //NOSAPI_DIGITALCONTENTDTO_H