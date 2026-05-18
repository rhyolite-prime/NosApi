//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#ifndef NOSAPI_SIGNINDTO_H
#define NOSAPI_SIGNINDTO_H

#include <json/json.h>

namespace nos::dto {

    class SigninDto {

    public:
        SigninDto() = default;
        explicit SigninDto(const Json::Value& json);

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

    inline void SigninDto::fromJson(const Json::Value& json) {

        if (json.isMember("usernameOrEmail") && !json["usernameOrEmail"].isNull()) {
            usernameOrEmail_ = json["usernameOrEmail"].asString();
        }

        if (json.isMember("password") && !json["password"].isNull()) {
            password_ = json["password"].asString();
        }

    }

}


#endif //NOSAPI_SIGNINDTO_H