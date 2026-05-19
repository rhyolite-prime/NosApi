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

        explicit WorkflowTransitionDto(const Json::Value& json);

        void fromJson(const Json::Value& json);

        // Getters
        [[nodiscard]] const std::string& getUsernameOrEmail() const { return usernameOrEmail_; }
        [[nodiscard]] const std::string& getPassword() const { return password_; }

        // Setters
        void setUsernameOrEmail(const std::string& usernameOrEmail) { usernameOrEmail_ = usernameOrEmail; }
        void setPassword(const std::string& password) { password_ = password; }

    private:
        std::string usernameOrEmail_;
        std::string password_;
    };

    inline void WorkflowTransitionDto::fromJson(const Json::Value& json) {

        if (json.isMember("usernameOrEmail") && !json["usernameOrEmail"].isNull()) {
            usernameOrEmail_ = json["usernameOrEmail"].asString();
        }

        if (json.isMember("password") && !json["password"].isNull()) {
            password_ = json["password"].asString();
        }

    }

}


#endif //NOSAPI_WORKFLOWTRANSITIONDTO_H