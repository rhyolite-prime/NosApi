//
// Created by Emmanuel Addo-Odame on 19/05/2026.
//

#ifndef NOSAPI_ACCESSLEVELDTO_H
#define NOSAPI_ACCESSLEVELDTO_H

#include <json/json.h>

namespace nos::dto {

    class AccessLevelDto {

    public:
        AccessLevelDto() = default;

        explicit AccessLevelDto(const Json::Value& json);

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getTitle() const { return title_; }
        [[nodiscard]] int getOrdering() const { return ordering_; }
        [[nodiscard]] const std::vector<std::string> &getRules() const { return rules_; }

        // Setters
        void setTitle(const std::string& title) { title_ = title; }
        void setOrdering(const int ordering) { ordering_ = ordering; }
        void setRules(const std::vector<std::string> &value) { rules_ = value; }

    private:
        std::string title_;
        int ordering_ = 0;
        std::vector<std::string> rules_;
    };

    inline void AccessLevelDto::fromJson(const Json::Value& json) {

        if (json.isMember("title") && !json["title"].isNull()) {
            title_ = json["title"].asString();
        }

        if (json.isMember("ordering") && !json["ordering"].isNull()) {
            ordering_ = json["ordering"].asInt();
        }

        if (json.isMember("rules") && json["rules"].isArray()) {
            for (const auto &ip : json["rules"]) {
                rules_.push_back(ip.asString());
            }
        }

    }

}


#endif //NOSAPI_ACCESSLEVELDTO_H