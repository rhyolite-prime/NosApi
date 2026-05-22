//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#include "AccessLevels.h"
#include "Users.h"
#include "dto/BaseApiResponse.h"
#include "UserService.h"
#include <jwt-cpp/jwt.h>
#include "bcrypt.h"
#include "constants/ErrorCodes.h"
#include "dto/RegisterUserPasskeysDto.h"
#include "utils/PasswordUtils.h"


using namespace drogon::orm;
using drogon_model::Nos::Users;

namespace nos::services {



drogon::Task<dto::BaseApiResponse> UserService::getAll(int pageNo, int pageSize, const std::string &query) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Users>(dbClient);

  // 1. Build the search criteria
  Criteria searchCriteria;
  if (!query.empty()) {
    std::string likeQuery = "%" + query + "%";
    searchCriteria =
        Criteria(Users::Cols::_first_name, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_email, CompareOperator::Like, likeQuery) ||
        Criteria(Users::Cols::_last_name, CompareOperator::Like, likeQuery);
  }

  dto::BaseApiResponse response;
  try {
    // 2. Get the total count matching the criteria
    size_t totalCount = co_await mp.count(searchCriteria);
    if (totalCount == 0) {
      response.success = true;
      response.result["data"] = Json::arrayValue;
      response.result["totalCount"] = 0;
      co_return response;
    }

    // 3. Find the paginated data
    int offset = (pageNo - 1) * pageSize;
    auto users = co_await mp.limit(pageSize).offset(offset).findBy(searchCriteria);

    // 4. Build the final response
    auto totalPages = (totalCount + pageSize - 1) / pageSize;
    response.success = true;
    response.result["totalCount"] = (Json::UInt64)totalCount;
    response.result["pageNo"] = pageNo;
    response.result["pageSize"] = pageSize;
    response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);
    response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
    response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount  : (Json::UInt64)(pageNo * pageSize);


    Json::Value data = Json::arrayValue;

    for (const auto &user : users) {

      Json::Value userJson = user.toJson();
      Json::Value camelCaseRole;
      camelCaseRole["id"] = userJson["id"];
      camelCaseRole["firstName"] = userJson["first_name"];
      camelCaseRole["lastName"] = userJson["last_name"];
      camelCaseRole["email"] = userJson["email"];
      camelCaseRole["phoneNumber"] = userJson["phone_number"];
      camelCaseRole["username"] = userJson["username"];
      camelCaseRole["country"] = userJson["country"];
      camelCaseRole["profileImageUrl"] = userJson["profile_image_url"];
      camelCaseRole["isLockedOut"] = userJson["is_locked_out"];
      camelCaseRole["isActive"] = userJson["is_active"];
      camelCaseRole["createdAt"] = userJson["created_at"];
      camelCaseRole["updatedAt"] = userJson["updated_at"];


      Json::Value userGroupsArray;
      Json::Reader reader;
      if (userJson.isMember("user_groups") && userJson["user_groups"].isString()) {
        reader.parse(userJson["user_groups"].asString(), userGroupsArray);
      }
      camelCaseRole["userGroups"] = userGroupsArray.isArray() ? userGroupsArray : Json::arrayValue;


      data.append(camelCaseRole);
    }
    response.result["data"] = data;

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.error["message"] = "Database error while fetching users.";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}


drogon::Task<dto::BaseApiResponse> UserService::create(const dto::UserDto &userDto) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Users>(dbClient);

  dto::BaseApiResponse response;
  try {
    Criteria duplicateCriteria =
        Criteria(Users::Cols::_username, CompareOperator::EQ, userDto.getUsername()) ||
        Criteria(Users::Cols::_email, CompareOperator::EQ, userDto.getEmail());

    size_t count = co_await mp.count(duplicateCriteria);
    if (count > 0) {
      response.success = false;
      response.message = "User with this username or email already exists";
      response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
      co_return response;
    }

    Users user;
    user.setId(drogon::utils::getUuid());
    user.setFirstName(userDto.getFirstName());
    user.setLastName(userDto.getLastName());
    user.setEmail(userDto.getEmail());
    user.setUsername(userDto.getUsername());
    user.setPhoneNumber(userDto.getPhoneNumber());
    user.setCountry(userDto.getCountry());
    
    user.setIsActive(true);
    user.setIsLockedOut(false);
    user.setRequireTwoFactorAuth(false);
    user.setSignCount(0);
    user.setCreatedAt(trantor::Date::now());
    user.setModifiedAt(trantor::Date::now());

    std::string password = userDto.getPassword();
    if (password.empty()) {
      password = nos::utils::PasswordUtils::generateRandomPassword(12);
    }
    std::string hashedPassword = bcrypt::generateHash(password);
    user.setPasswordHash(hashedPassword);

    Json::Value permissionsArray = Json::arrayValue;
    for (const auto& perm : userDto.getUserGroups()) {
      permissionsArray.append(perm);
    }
    Json::FastWriter writer;
    std::string userGroupsStr = writer.write(permissionsArray);
    if (!userGroupsStr.empty() && userGroupsStr.back() == '\n') {
      userGroupsStr.pop_back();
    }

    user.setUserGroups(userGroupsStr);

    co_await mp.insert(user);

    response.success = true;
    response.message = "User created successfully";
    response.result["userId"] = user.getValueOfId();

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to create user";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}


drogon::Task<dto::BaseApiResponse> UserService::update(const dto::UserDto &userDto, const std::string &userId) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Users>(dbClient);

  dto::BaseApiResponse response;
  try {
    auto user = co_await mp.findOne(Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    if (!userDto.getFirstName().empty()) {
      user.setFirstName(userDto.getFirstName());
    }
    if (!userDto.getLastName().empty()) {
      user.setLastName(userDto.getLastName());
    }
    if (!userDto.getEmail().empty()) {
      if (userDto.getEmail() != user.getValueOfEmail()) {
        size_t emailCount = co_await mp.count(Criteria(Users::Cols::_email, CompareOperator::EQ, userDto.getEmail()));
        if (emailCount > 0) {
          response.success = false;
          response.message = "Email is already in use by another user";
          response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
          co_return response;
        }
      }
      user.setEmail(userDto.getEmail());
    }
    if (!userDto.getUsername().empty()) {

      if (userDto.getUsername() != user.getValueOfUsername()) {
        size_t usernameCount = co_await mp.count(Criteria(Users::Cols::_username, CompareOperator::EQ, userDto.getUsername()));
        if (usernameCount > 0) {
          response.success = false;
          response.message = "Username is already in use by another user";
          response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
          co_return response;
        }
      }

      user.setUsername(userDto.getUsername());
    }
    if (!userDto.getPhoneNumber().empty()) {
      user.setPhoneNumber(userDto.getPhoneNumber());
    }
    if (!userDto.getCountry().empty()) {
      user.setCountry(userDto.getCountry());
    }
    if (!userDto.getPassword().empty()) {
      std::string hashedPassword = bcrypt::generateHash(userDto.getPassword());
      user.setPasswordHash(hashedPassword);

      //send an email notifying password change.
    }

    Json::Value permissionsArray = Json::arrayValue;
    for (const auto& perm : userDto.getUserGroups()) {
      permissionsArray.append(perm);
    }
    Json::FastWriter writer;
    std::string userGroupsStr = writer.write(permissionsArray);
    if (!userGroupsStr.empty() && userGroupsStr.back() == '\n') {
      userGroupsStr.pop_back();
    }

    user.setUserGroups(userGroupsStr);

    user.setModifiedAt(trantor::Date::now());

    co_await mp.update(user);

    response.success = true;
    response.message = "User updated successfully";

  } catch (const UnexpectedRows &e) {
    response.success = false;
    response.message = "User not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to update user";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}


drogon::Task<dto::BaseApiResponse> UserService::deleteUser(const std::string &userId) {
  auto dbClient = drogon::app().getDbClient();
  auto mp = CoroMapper<Users>(dbClient);

  dto::BaseApiResponse response;
  try {
    auto count = co_await mp.deleteBy(Criteria(Users::Cols::_id, CompareOperator::EQ, userId));

    if (count > 0) {
      response.success = true;
      response.message = "User deleted successfully";
    } else {
      response.success = false;
      response.message = "User not found";
      response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    }

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Failed to delete user";
    response.error["detail"] = e.base().what();
  }
  co_return response;
}


drogon::Task<dto::BaseApiResponse> UserService::validateAdminUserCredentials(const dto::SigninDto &signin_dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mapper(dbClient);

  Criteria criteria =
      (Criteria(Users::Cols::_username, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail()) ||
       Criteria(Users::Cols::_email, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail())) &&
      Criteria(Users::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Users::Cols::_is_locked_out, CompareOperator::EQ, false);

  nos::dto::BaseApiResponse response;
  try {
    Users user = co_await mapper.findOne(criteria);
    bool passwordMatches = bcrypt::validatePassword(
        signin_dto.getPassword(), user.getValueOfPasswordHash());

    if (passwordMatches) {

      Users userToUpdate = user;
      userToUpdate.setLastActive(trantor::Date::now());
      co_await mapper.update(userToUpdate);

      // Password is correct, generate JWT token
      auto &app = drogon::app();
      auto customConfig = app.getCustomConfig();
      std::string jwtSecurityKey =
          customConfig["JwtBearer"]["JwtSecurityKey"].asString();
      std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

      auto token =
          jwt::create()
              .set_issuer(jwtIssuer)
              .set_type("JWT")
              .set_issued_at(std::chrono::system_clock::now())
              .set_expires_at(std::chrono::system_clock::now() +
                              std::chrono::hours(24 * 30))
              .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
              .set_payload_claim("username",
                                 jwt::claim(user.getValueOfUsername()))
              .set_payload_claim("email", jwt::claim(user.getValueOfEmail()))
              .sign(jwt::algorithm::hs256{jwtSecurityKey});

      response.success = true;
      response.message = "Authentication successful";
      response.result["token"] = token;
      response.result["userId"] = user.getValueOfId();
      response.result["username"] = user.getValueOfUsername();
      response.result["fullName"] =
          user.getValueOfFirstName() + " " + user.getValueOfLastName();
      response.result["email"] = user.getValueOfEmail();
    } else {
      // Password is incorrect
      response.success = false;
      response.message = "Invalid credentials";
      response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
    }
  } catch (const DrogonDbException &e) {
    // Database error or user not found
    response.success = false;
    response.message = "User not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "User not found";
  }
  co_return response;
}


drogon::Task<dto::BaseApiResponse> UserService::validateUserPasskeys(const dto::UserPasskeyDto &passkeyDto) {

      auto dbClient = drogon::app().getDbClient();
      CoroMapper<Users> mp(dbClient);

      auto toStandardBase64 = [](std::string s) {
        for (char &c : s) {
          if (c == '-')
            c = '+';
          else if (c == '_')
            c = '/';
        }
        while (s.length() % 4 != 0)
          s += '=';
        return s;
      };

      std::string credIdStr = toStandardBase64(passkeyDto.getCredentialId());
      std::vector<char> credIdBytes = drogon::utils::base64DecodeToVector(credIdStr);

      std::string userId = passkeyDto.getUserHandle();

      Criteria userCriteria;
      bool hasUserHandle = !userId.empty();

      dto::BaseApiResponse response;
      if (hasUserHandle) {
        if (userId.length() > 36) { // rudimentary check
          std::string handleStr = toStandardBase64(userId);
          auto handleBytes = drogon::utils::base64DecodeToVector(handleStr);
          userId = std::string(handleBytes.begin(), handleBytes.end());
        }
        userCriteria = Criteria(Users::Cols::_id, CompareOperator::EQ, userId);
      } else {
        response.success = false;
        response.message = "User Handle is required for passkey login.";
        co_return response;
      }

      try {
        Users user = co_await mp.findOne(userCriteria);
        // Found user. Now verify passkey.

        // 1. Verify Credential ID matches (if we found by userHandle)
        auto storedCredId = user.getValueOfCredentialId();
        if (credIdBytes != storedCredId) {
          // Passkey doesn't belong to this user or changed
          response.success = false;
          response.message = "Invalid credential ID.";
          co_return response;
        }

    // 2. Cryptographic Verification
    // SignedData = authenticatorData + sha256(clientDataJSON)

        std::string authDataStr =
            toStandardBase64(passkeyDto.getAuthenticatorData());
        std::vector<char> authData =
            drogon::utils::base64DecodeToVector(authDataStr);

        std::string clientDataStr =
            toStandardBase64(passkeyDto.getClientDataJSON());
        std::vector<char> clientData = drogon::utils::base64DecodeToVector(clientDataStr);

        unsigned char clientDataHash[SHA256_DIGEST_LENGTH];
        SHA256((const unsigned char *)clientData.data(), clientData.size(),
               clientDataHash);

        std::vector<unsigned char> signedData;
        signedData.reserve(authData.size() + SHA256_DIGEST_LENGTH);
        signedData.insert(signedData.end(), authData.begin(), authData.end());
        signedData.insert(signedData.end(), clientDataHash,
                          clientDataHash + SHA256_DIGEST_LENGTH);

    // Public Key from DB
    auto pubKeyBytes = user.getValueOfPublicKey();

    const unsigned char *p = (const unsigned char *)pubKeyBytes.data();
    EVP_PKEY *pkey = d2i_PUBKEY(NULL, &p, pubKeyBytes.size());

    if (!pkey) {
      response.success = false;
      response.message = "Failed to load stored public key.";
      co_return response;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey);

    // Verification
    // signature from DTO
    std::string sigStr = toStandardBase64(passkeyDto.getSignature());
    std::vector<char> sigBytes = drogon::utils::base64DecodeToVector(sigStr);

    int verifyResult =
        EVP_DigestVerify(ctx, (const unsigned char *)sigBytes.data(),
                         sigBytes.size(), signedData.data(), signedData.size());

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    if (verifyResult != 1) {
      response.success = false;
      response.message = "Signature verification failed.";
      co_return response;
    }

    // 3. Clone Check (Sign Count)
    // Extract signCount from authData (bytes 33-36)
    if (authData.size() < 37) {
      response.success = false;
      response.message = "Authenticator data too short.";
      co_return response;
    }

    uint32_t newSignCount =
        ((uint8_t)authData[33] << 24) | ((uint8_t)authData[34] << 16) |
        ((uint8_t)authData[35] << 8) | (uint8_t)authData[36];

    int64_t storedCount = user.getValueOfSignCount();

    if (newSignCount > 0 && newSignCount <= storedCount) {
      // Potential clone attack!
      response.success = false;
      response.message = "Invalid sign count (possible clone detected).";
      co_return response;
    }

    // 4. Update Sign Count & Issue Token
    Users userToUpdate = user;
    userToUpdate.setSignCount(newSignCount);

    co_await mp.update(userToUpdate);

    // Issue Token
    auto &app = drogon::app();
    auto customConfig = app.getCustomConfig();
    std::string jwtSecurityKey =
        customConfig["JwtBearer"]["JwtSecurityKey"].asString();
    std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

    auto token =
        jwt::create()
            .set_issuer(jwtIssuer)
            .set_type("JWT")
            .set_issued_at(std::chrono::system_clock::now())
            .set_expires_at(std::chrono::system_clock::now() +
                            std::chrono::hours(24 * 120))
            .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
            .set_payload_claim("username",
                               jwt::claim(user.getValueOfUsername()))
            .set_payload_claim("email", jwt::claim(user.getValueOfEmail()))
            .sign(jwt::algorithm::hs256{jwtSecurityKey});

    response.success = true;
    response.message = "Authentication successful";
    response.result["token"] = token;
    response.result["userId"] = user.getValueOfId();
    response.result["username"] = user.getValueOfUsername();
    response.result["fullName"] =
        user.getValueOfFirstName() + " " + user.getValueOfLastName();
    response.result["email"] = user.getValueOfEmail();

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "User not found or invalid credentials.";
  }
  co_return response;
}


drogon::Task<dto::BaseApiResponse> UserService::validateUserCredentials(const dto::SigninDto &signin_dto) {
  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mapper(dbClient);

  Criteria criteria =
      (Criteria(Users::Cols::_username, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail()) ||
       Criteria(Users::Cols::_email, CompareOperator::EQ,
                signin_dto.getUsernameOrEmail())) &&
      Criteria(Users::Cols::_is_active, CompareOperator::EQ, true) &&
      Criteria(Users::Cols::_is_locked_out, CompareOperator::EQ, false);

  dto::BaseApiResponse response;
  try {
    Users user = co_await mapper.findOne(criteria);

    std::string storedHash = nos::utils::PasswordUtils::normalizeBcryptHash(
        user.getValueOfPasswordHash());

    bool passwordMatches =
        bcrypt::validatePassword(signin_dto.getPassword(), storedHash);


    if (passwordMatches) {

      Users userToUpdate = user;
      userToUpdate.setLastActive(trantor::Date::now());
      co_await mapper.update(userToUpdate);

      // Password is correct, generate JWT token
      auto &app = drogon::app();
      auto customConfig = app.getCustomConfig();
      std::string jwtSecurityKey =
          customConfig["JwtBearer"]["JwtSecurityKey"].asString();
      std::string jwtIssuer = customConfig["JwtBearer"]["JwtIssuer"].asString();

      auto token =
          jwt::create()
              .set_issuer(jwtIssuer)
              .set_type("JWT")
              .set_issued_at(std::chrono::system_clock::now())
              .set_expires_at(std::chrono::system_clock::now() +
                              std::chrono::hours(24 * 120))
              .set_payload_claim("userId", jwt::claim(user.getValueOfId()))
              .set_payload_claim("username",
                                 jwt::claim(user.getValueOfUsername()))
              .set_payload_claim("email", jwt::claim(user.getValueOfEmail()))
              .sign(jwt::algorithm::hs256{jwtSecurityKey});

      response.success = true;
      response.message = "Authentication successful";
      response.result["token"] = token;
      response.result["userId"] = user.getValueOfId();
      response.result["username"] = user.getValueOfUsername();
      response.result["fullName"] =
          user.getValueOfFirstName() + " " + user.getValueOfLastName();
      response.result["email"] = user.getValueOfEmail();
    } else {
      // Password is incorrect
      response.success = false;
      response.message = "Invalid credentials";
      response.error["code"] = constants::ERR_AUTH_INVALID_CREDENTIALS;
    }
  } catch (const DrogonDbException &e) {
    // User not found
    response.success = false;
    response.message = "User not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
    response.error["message"] = "User not found";
  }
  co_return response;
}


drogon::Task<dto::BaseApiResponse> UserService::registerUserPasskeys(const dto::RegisterUserPasskeysDto &passKeysDto) {

  auto dbClient = drogon::app().getDbClient();
  CoroMapper<Users> mp(dbClient);

  nos::dto::BaseApiResponse response;
  try {
    Users foundUser = co_await mp.findOne(Criteria(
        Users::Cols::_id, CompareOperator::EQ, passKeysDto.getUserId()));

    Users user = foundUser;

    // Helper to ensure Base64 is standard (url-safe replacement)
    auto toStandardBase64 = [](std::string s) {
      for (char &c : s) {
        if (c == '-')
          c = '+';
        else if (c == '_')
          c = '/';
      }
      while (s.length() % 4 != 0)
        s += '=';
      return s;
    };

    std::string credIdStr = toStandardBase64(passKeysDto.getCredentialId());
    std::string pubKeyStr = toStandardBase64(passKeysDto.getPublicKey());

    user.setCredentialId(drogon::utils::base64DecodeToVector(credIdStr));
    user.setPublicKey(drogon::utils::base64DecodeToVector(pubKeyStr));
    user.setPublicKeyAlgorithm(passKeysDto.getPublicKeyAlgorithm());

    // Passkey fields
    // Extract signCount from attestationObject (CBOR map) if available
    int64_t initialSignCount = 0;

    try {
      std::string attObjStr =
          toStandardBase64(passKeysDto.getAttestationObject());
      std::vector<char> attObjBytes =
          drogon::utils::base64DecodeToVector(attObjStr);

      // Simple CBOR 'authData' finder
      const std::string authDataKey = "authData";
      const char authDataKeyHeader = 0x60 | (char)authDataKey.length(); // 0x68

      auto it = std::search(attObjBytes.begin(), attObjBytes.end(),
                            authDataKey.begin(), authDataKey.end());

      if (it != attObjBytes.end() && it != attObjBytes.begin()) {
        if (*(it - 1) == authDataKeyHeader) {
          auto valueStart = it + authDataKey.length();
          if (valueStart < attObjBytes.end()) {
            size_t dataLen = 0;
            auto dataIt = valueStart;
            uint8_t head = (uint8_t)*dataIt;
            dataIt++;

            if (head >= 0x40 && head <= 0x57) {
              dataLen = head - 0x40;
            } else if (head == 0x58) {
              if (dataIt < attObjBytes.end()) {
                dataLen = (uint8_t)*dataIt;
                dataIt++;
              }
            } else if (head == 0x59) {
              if (dataIt + 1 < attObjBytes.end()) {
                dataLen = ((uint8_t)*dataIt << 8) | (uint8_t) * (dataIt + 1);
                dataIt += 2;
              }
            }

            if (dataLen >= 37 &&
                (size_t)std::distance(dataIt, attObjBytes.end()) >= dataLen) {
              auto scIt = dataIt + 32 + 1; // start of signCount
              uint32_t sc =
                  ((uint8_t)*scIt << 24) | ((uint8_t) * (scIt + 1) << 16) |
                  ((uint8_t) * (scIt + 2) << 8) | (uint8_t) * (scIt + 3);
              initialSignCount = sc;
            }
          }
        }
      }
    } catch (...) {
      // Log or handle parsing error if necessary, but don't fail registration
    }

    user.setSignCount(initialSignCount);
    user.setCredentialType(passKeysDto.getCredentialType());
    user.setTransports(passKeysDto.getTransports());
    user.setModifiedAt(trantor::Date::now());

    std::string uId = user.getValueOfId();
    std::vector<char> handleVec(uId.begin(), uId.end());
    user.setUserHandle(handleVec);

    co_await mp.update(user);

    response.success = true;
    response.message = "Passkeys registered successfully";

  } catch (const DrogonDbException &e) {
    response.success = false;
    response.message = "Database error or user not found";
    response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
  }
  co_return response;
}




}