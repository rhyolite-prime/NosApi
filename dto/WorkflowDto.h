//
// Created by Emmanuel Addo-Odame on 19/05/2026.
//

#ifndef NOSAPI_WORKFLOWDTO_H
#define NOSAPI_WORKFLOWDTO_H

#include <json/json.h>

namespace nos::dto {

    class WorkflowDto {

    public:
        WorkflowDto() = default;
        explicit WorkflowDto(const Json::Value& json);

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

    inline void WorkflowDto::fromJson(const Json::Value& json) {

        if (json.isMember("usernameOrEmail") && !json["usernameOrEmail"].isNull()) {
            usernameOrEmail_ = json["usernameOrEmail"].asString();
        }

        if (json.isMember("password") && !json["password"].isNull()) {
            password_ = json["password"].asString();
        }

    }

}



#endif //NOSAPI_WORKFLOWDTO_H