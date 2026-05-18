//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#ifndef NOSAPI_PASSWORDUTILS_H
#define NOSAPI_PASSWORDUTILS_H

#include <string>

namespace nos::utils {

    class PasswordUtils {
    public:
        /**
         * @brief Generates a random alphanumeric password
         * @param length The length of the password to generate (default: 8)
         * @return A random password string
         */
        static std::string generateRandomPassword(int length = 8);
        static std::string normalizeBcryptHash(std::string hash);
    };

}

#endif //NOSAPI_PASSWORDUTILS_H