//
// Created by Emmanuel Addo-Odame on 17/05/2026.
//

#include "DigitalContentService.h"
#include "DigitalContents.h"
#include "constants/ErrorCodes.h"
#include <json/json.h>
#include <algorithm>
#include <cctype>
#include <cstring>

using namespace drogon::orm;
using drogon_model::Nos::DigitalContents;

namespace nos::services {

    inline std::string generateAlias(const std::string &title) {
        std::string alias = "";
        for (char c : title) {
            if (std::isalnum(c)) {
                alias += std::tolower(c);
            } else if (c == ' ' || c == '_' || c == '-') {
                if (alias.empty() || alias.back() != '-') {
                    alias += '-';
                }
            }
        }
        if (!alias.empty() && alias.back() == '-') {
            alias.pop_back();
        }
        if (alias.empty()) {
            alias = "content";
        }
        return alias;
    }

    inline std::shared_ptr<trantor::Date> parseDateTime(const std::string &timeStr) {
        if (timeStr.empty()) return nullptr;
        struct tm stm;
        memset(&stm, 0, sizeof(stm));
        const char *p = nullptr;
        if (timeStr.find('T') != std::string::npos) {
            p = strptime(timeStr.c_str(), "%Y-%m-%dT%H:%M:%S", &stm);
        } else {
            p = strptime(timeStr.c_str(), "%Y-%m-%d %H:%M:%S", &stm);
        }
        if (!p) return nullptr;

        time_t t = mktime(&stm);
        size_t decimalNum = 0;
        if (*p == '.') {
            std::string decimals(p + 1, &timeStr[timeStr.length()]);
            while (decimals.length() < 6) {
                decimals += "0";
            }
            decimalNum = (size_t)atol(decimals.c_str());
        }
        return std::make_shared<trantor::Date>(t * 1000000 + decimalNum);
    }

    drogon::Task<dto::BaseApiResponse> DigitalContentService::getAll(int pageNo, int pageSize, const std::string &query) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<DigitalContents>(dbClient);

        Criteria searchCriteria;
        if (!query.empty()) {
            std::string likeQuery = "%" + query + "%";
            searchCriteria = Criteria(DigitalContents::Cols::_title, CompareOperator::Like, likeQuery) ||
                             Criteria(DigitalContents::Cols::_introtext, CompareOperator::Like, likeQuery) ||
                             Criteria(DigitalContents::Cols::_fulltext, CompareOperator::Like, likeQuery);
        }

        dto::BaseApiResponse response;
        try {
            size_t totalCount = co_await mp.count(searchCriteria);
            if (totalCount == 0) {
                response.success = true;
                response.result["data"] = Json::arrayValue;
                response.result["totalCount"] = 0;
                co_return response;
            }

            int offset = (pageNo - 1) * pageSize;
            auto contents = co_await mp.limit(pageSize).offset(offset).orderBy(DigitalContents::Cols::_ordering, SortOrder::ASC).findBy(searchCriteria);

            auto totalPages = (totalCount + pageSize - 1) / pageSize;

            response.success = true;
            response.result["totalCount"] = (Json::UInt64)totalCount;
            response.result["pageNo"] = pageNo;
            response.result["pageSize"] = pageSize;
            response.result["totalPages"] = (int)((totalCount + pageSize - 1) / pageSize);
            response.result["lowerBound"] = pageSize * (pageNo - 1) + 1;
            response.result["upperBound"] = (int)totalPages == pageNo ? (Json::UInt64)totalCount : (Json::UInt64)(pageNo * pageSize);

            Json::Value data = Json::arrayValue;
            for (const auto &content : contents) {
                Json::Value contentJson = content.toJson();
                Json::Value camelCaseContent;
                camelCaseContent["id"] = contentJson["id"];
                camelCaseContent["title"] = contentJson["title"];
                camelCaseContent["alias"] = contentJson["alias"];
                camelCaseContent["contentTypeId"] = contentJson["content_type_id"];
                camelCaseContent["contentType"] = contentJson["content_type"];
                camelCaseContent["introtext"] = contentJson["introtext"];
                camelCaseContent["fulltext"] = contentJson["fulltext"];
                camelCaseContent["primaryResourceId"] = contentJson["primary_resource_id"];
                camelCaseContent["status"] = contentJson["status"];
                camelCaseContent["categoryId"] = contentJson["category_id"];
                camelCaseContent["ordering"] = contentJson["ordering"];
                camelCaseContent["featured"] = contentJson["featured"];
                camelCaseContent["accessLevelId"] = contentJson["access_level_id"];
                camelCaseContent["metakey"] = contentJson["metakey"];
                camelCaseContent["metadesc"] = contentJson["metadesc"];

                Json::Reader reader;
                Json::Value metadataJson = Json::objectValue;
                if (contentJson.isMember("metadata") && contentJson["metadata"].isString()) {
                    reader.parse(contentJson["metadata"].asString(), metadataJson);
                }
                camelCaseContent["metadata"] = metadataJson;

                Json::Value presentationConfigJson = Json::objectValue;
                if (contentJson.isMember("presentation_config") && contentJson["presentation_config"].isString()) {
                    reader.parse(contentJson["presentation_config"].asString(), presentationConfigJson);
                }
                camelCaseContent["presentationConfig"] = presentationConfigJson;

                Json::Value displayOverridesJson = Json::objectValue;
                if (contentJson.isMember("display_overrides") && contentJson["display_overrides"].isString()) {
                    reader.parse(contentJson["display_overrides"].asString(), displayOverridesJson);
                }
                camelCaseContent["displayOverrides"] = displayOverridesJson;

                camelCaseContent["publishUp"] = contentJson["publish_up"];
                camelCaseContent["publishDown"] = contentJson["publish_down"];
                camelCaseContent["version"] = contentJson["version"];
                camelCaseContent["language"] = contentJson["language"];
                camelCaseContent["createdBy"] = contentJson["created_by"];
                camelCaseContent["createdAt"] = contentJson["created_at"];
                camelCaseContent["modifiedBy"] = contentJson["modified_by"];
                camelCaseContent["modifiedAt"] = contentJson["modified_at"];
                camelCaseContent["views"] = contentJson["views"];

                data.append(camelCaseContent);
            }
            response.result["data"] = data;

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.error["message"] = "Database error while fetching digital contents.";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> DigitalContentService::getDetails(const std::string &id) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<DigitalContents>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto content = co_await mp.findOne(Criteria(DigitalContents::Cols::_id, CompareOperator::EQ, id));

            Json::Value contentJson = content.toJson();
            Json::Value camelCaseContent;
            camelCaseContent["id"] = contentJson["id"];
            camelCaseContent["title"] = contentJson["title"];
            camelCaseContent["alias"] = contentJson["alias"];
            camelCaseContent["contentTypeId"] = contentJson["content_type_id"];
            camelCaseContent["contentType"] = contentJson["content_type"];
            camelCaseContent["introtext"] = contentJson["introtext"];
            camelCaseContent["fulltext"] = contentJson["fulltext"];
            camelCaseContent["primaryResourceId"] = contentJson["primary_resource_id"];
            camelCaseContent["status"] = contentJson["status"];
            camelCaseContent["categoryId"] = contentJson["category_id"];
            camelCaseContent["ordering"] = contentJson["ordering"];
            camelCaseContent["featured"] = contentJson["featured"];
            camelCaseContent["accessLevelId"] = contentJson["access_level_id"];
            camelCaseContent["metakey"] = contentJson["metakey"];
            camelCaseContent["metadesc"] = contentJson["metadesc"];

            Json::Reader reader;
            Json::Value metadataJson = Json::objectValue;
            if (contentJson.isMember("metadata") && contentJson["metadata"].isString()) {
                reader.parse(contentJson["metadata"].asString(), metadataJson);
            }
            camelCaseContent["metadata"] = metadataJson;

            Json::Value presentationConfigJson = Json::objectValue;
            if (contentJson.isMember("presentation_config") && contentJson["presentation_config"].isString()) {
                reader.parse(contentJson["presentation_config"].asString(), presentationConfigJson);
            }
            camelCaseContent["presentationConfig"] = presentationConfigJson;

            Json::Value displayOverridesJson = Json::objectValue;
            if (contentJson.isMember("display_overrides") && contentJson["display_overrides"].isString()) {
                reader.parse(contentJson["display_overrides"].asString(), displayOverridesJson);
            }
            camelCaseContent["displayOverrides"] = displayOverridesJson;

            camelCaseContent["publishUp"] = contentJson["publish_up"];
            camelCaseContent["publishDown"] = contentJson["publish_down"];
            camelCaseContent["version"] = contentJson["version"];
            camelCaseContent["language"] = contentJson["language"];
            camelCaseContent["createdBy"] = contentJson["created_by"];
            camelCaseContent["createdAt"] = contentJson["created_at"];
            camelCaseContent["modifiedBy"] = contentJson["modified_by"];
            camelCaseContent["modifiedAt"] = contentJson["modified_at"];
            camelCaseContent["views"] = contentJson["views"];

            response.success = true;
            response.result["data"] = camelCaseContent;

        } catch (const UnexpectedRows &e) {
            response.success = false;
            response.message = "Digital content not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Database error while fetching digital content details.";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> DigitalContentService::create(const dto::DigitalContentDto &digitalContentDto) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<DigitalContents>(dbClient);

        dto::BaseApiResponse response;
        try {
            std::string alias = digitalContentDto.hasAlias() && !digitalContentDto.getAlias().empty() ? 
                                digitalContentDto.getAlias() : generateAlias(digitalContentDto.getTitle());

            size_t count = co_await mp.count(Criteria(DigitalContents::Cols::_alias, CompareOperator::EQ, alias));
            if (count > 0) {
                response.success = false;
                response.message = "Digital content with this alias already exists";
                response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
                co_return response;
            }

            DigitalContents content;
            content.setTitle(digitalContentDto.getTitle());
            content.setAlias(alias);

            if (digitalContentDto.hasContentTypeId() && !digitalContentDto.getContentTypeId().empty()) {
                content.setContentTypeId(digitalContentDto.getContentTypeId());
            } else {
                content.setContentTypeIdToNull();
            }

            if (digitalContentDto.hasContentType() && !digitalContentDto.getContentType().empty()) {
                content.setContentType(digitalContentDto.getContentType());
            } else {
                content.setContentTypeToNull();
            }

            if (digitalContentDto.hasIntrotext()) {
                content.setIntrotext(digitalContentDto.getIntrotext());
            } else {
                content.setIntrotextToNull();
            }

            if (digitalContentDto.hasFulltext()) {
                content.setFulltext(digitalContentDto.getFulltext());
            } else {
                content.setFulltextToNull();
            }

            if (digitalContentDto.hasPrimaryResourceId() && !digitalContentDto.getPrimaryResourceId().empty()) {
                content.setPrimaryResourceId(digitalContentDto.getPrimaryResourceId());
            } else {
                content.setPrimaryResourceIdToNull();
            }

            content.setStatus(digitalContentDto.getStatus());

            if (digitalContentDto.hasCategoryId() && !digitalContentDto.getCategoryId().empty()) {
                content.setCategoryId(digitalContentDto.getCategoryId());
            } else {
                content.setCategoryIdToNull();
            }

            content.setOrdering(digitalContentDto.getOrdering());
            content.setFeatured(digitalContentDto.getFeatured());

            if (digitalContentDto.hasAccessLevelId() && !digitalContentDto.getAccessLevelId().empty()) {
                content.setAccessLevelId(digitalContentDto.getAccessLevelId());
            } else {
                content.setAccessLevelIdToNull();
            }

            if (digitalContentDto.hasMetakey()) {
                content.setMetakey(digitalContentDto.getMetakey());
            } else {
                content.setMetakeyToNull();
            }

            if (digitalContentDto.hasMetadesc()) {
                content.setMetadesc(digitalContentDto.getMetadesc());
            } else {
                content.setMetadescToNull();
            }

            Json::FastWriter writer;
            if (digitalContentDto.hasMetadata()) {
                std::string metadataStr = writer.write(digitalContentDto.getMetadata());
                if (!metadataStr.empty() && metadataStr.back() == '\n') metadataStr.pop_back();
                content.setMetadata(metadataStr);
            } else {
                content.setMetadata("{}");
            }

            if (digitalContentDto.hasPresentationConfig()) {
                std::string presConfigStr = writer.write(digitalContentDto.getPresentationConfig());
                if (!presConfigStr.empty() && presConfigStr.back() == '\n') presConfigStr.pop_back();
                content.setPresentationConfig(presConfigStr);
            } else {
                content.setPresentationConfig("{}");
            }

            if (digitalContentDto.hasDisplayOverrides()) {
                std::string dispOverridesStr = writer.write(digitalContentDto.getDisplayOverrides());
                if (!dispOverridesStr.empty() && dispOverridesStr.back() == '\n') dispOverridesStr.pop_back();
                content.setDisplayOverrides(dispOverridesStr);
            } else {
                content.setDisplayOverrides("{}");
            }

            if (digitalContentDto.hasPublishUp() && !digitalContentDto.getPublishUp().empty()) {
                auto pubUp = parseDateTime(digitalContentDto.getPublishUp());
                if (pubUp) {
                    content.setPublishUp(*pubUp);
                } else {
                    content.setPublishUpToNull();
                }
            } else {
                content.setPublishUpToNull();
            }

            if (digitalContentDto.hasPublishDown() && !digitalContentDto.getPublishDown().empty()) {
                auto pubDown = parseDateTime(digitalContentDto.getPublishDown());
                if (pubDown) {
                    content.setPublishDown(*pubDown);
                } else {
                    content.setPublishDownToNull();
                }
            } else {
                content.setPublishDownToNull();
            }

            content.setVersion(digitalContentDto.hasVersion() ? digitalContentDto.getVersion() : 1);
            content.setLanguage(digitalContentDto.hasLanguage() ? digitalContentDto.getLanguage() : "*");

            if (digitalContentDto.hasCreatedBy() && !digitalContentDto.getCreatedBy().empty()) {
                content.setCreatedBy(digitalContentDto.getCreatedBy());
            } else {
                content.setCreatedByToNull();
            }

            content.setCreatedAt(trantor::Date::now());

            if (digitalContentDto.hasModifiedBy() && !digitalContentDto.getModifiedBy().empty()) {
                content.setModifiedBy(digitalContentDto.getModifiedBy());
            } else {
                content.setModifiedByToNull();
            }

            content.setModifiedAt(trantor::Date::now());
            content.setViews(digitalContentDto.getViews());

            co_await mp.insert(content);

            response.success = true;
            response.message = "Digital content created successfully";
            response.result["digitalContentId"] = content.getValueOfId();

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to create digital content";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> DigitalContentService::update(const dto::DigitalContentDto &digitalContentDto, const std::string &id) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<DigitalContents>(dbClient);


        dto::BaseApiResponse response;
        try {
            if (id.empty()) {
                response.success = false;
                response.message = "Digital content ID is required for update";
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
                co_return response;
            }

            auto content = co_await mp.findOne(Criteria(DigitalContents::Cols::_id, CompareOperator::EQ, id));

            if (digitalContentDto.hasTitle()) {
                content.setTitle(digitalContentDto.getTitle());
            }

            if (digitalContentDto.hasAlias()) {
                std::string alias = digitalContentDto.getAlias();
                if (alias.empty()) {
                    alias = generateAlias(content.getValueOfTitle());
                }
                if (alias != content.getValueOfAlias()) {
                    size_t count = co_await mp.count(Criteria(DigitalContents::Cols::_alias, CompareOperator::EQ, alias));
                    if (count > 0) {
                        response.success = false;
                        response.message = "Digital content with this alias already exists";
                        response.error["code"] = constants::ERR_DUPLICATE_RESOURCE;
                        co_return response;
                    }
                }
                content.setAlias(alias);
            }

            if (digitalContentDto.hasContentTypeId()) {
                if (!digitalContentDto.getContentTypeId().empty()) {
                    content.setContentTypeId(digitalContentDto.getContentTypeId());
                } else {
                    content.setContentTypeIdToNull();
                }
            }

            if (digitalContentDto.hasContentType()) {
                if (!digitalContentDto.getContentType().empty()) {
                    content.setContentType(digitalContentDto.getContentType());
                } else {
                    content.setContentTypeToNull();
                }
            }

            if (digitalContentDto.hasIntrotext()) {
                if (!digitalContentDto.getIntrotext().empty()) {
                    content.setIntrotext(digitalContentDto.getIntrotext());
                } else {
                    content.setIntrotextToNull();
                }
            }

            if (digitalContentDto.hasFulltext()) {
                if (!digitalContentDto.getFulltext().empty()) {
                    content.setFulltext(digitalContentDto.getFulltext());
                } else {
                    content.setFulltextToNull();
                }
            }

            if (digitalContentDto.hasPrimaryResourceId()) {
                if (!digitalContentDto.getPrimaryResourceId().empty()) {
                    content.setPrimaryResourceId(digitalContentDto.getPrimaryResourceId());
                } else {
                    content.setPrimaryResourceIdToNull();
                }
            }

            if (digitalContentDto.hasStatus()) {
                content.setStatus(digitalContentDto.getStatus());
            }

            if (digitalContentDto.hasCategoryId()) {
                if (!digitalContentDto.getCategoryId().empty()) {
                    content.setCategoryId(digitalContentDto.getCategoryId());
                } else {
                    content.setCategoryIdToNull();
                }
            }

            if (digitalContentDto.hasOrdering()) {
                content.setOrdering(digitalContentDto.getOrdering());
            }

            if (digitalContentDto.hasFeatured()) {
                content.setFeatured(digitalContentDto.getFeatured());
            }

            if (digitalContentDto.hasAccessLevelId()) {
                if (!digitalContentDto.getAccessLevelId().empty()) {
                    content.setAccessLevelId(digitalContentDto.getAccessLevelId());
                } else {
                    content.setAccessLevelIdToNull();
                }
            }

            if (digitalContentDto.hasMetakey()) {
                if (!digitalContentDto.getMetakey().empty()) {
                    content.setMetakey(digitalContentDto.getMetakey());
                } else {
                    content.setMetakeyToNull();
                }
            }

            if (digitalContentDto.hasMetadesc()) {
                if (!digitalContentDto.getMetadesc().empty()) {
                    content.setMetadesc(digitalContentDto.getMetadesc());
                } else {
                    content.setMetadescToNull();
                }
            }

            Json::FastWriter writer;
            if (digitalContentDto.hasMetadata()) {
                std::string metadataStr = writer.write(digitalContentDto.getMetadata());
                if (!metadataStr.empty() && metadataStr.back() == '\n') metadataStr.pop_back();
                content.setMetadata(metadataStr);
            }

            if (digitalContentDto.hasPresentationConfig()) {
                std::string presConfigStr = writer.write(digitalContentDto.getPresentationConfig());
                if (!presConfigStr.empty() && presConfigStr.back() == '\n') presConfigStr.pop_back();
                content.setPresentationConfig(presConfigStr);
            }

            if (digitalContentDto.hasDisplayOverrides()) {
                std::string dispOverridesStr = writer.write(digitalContentDto.getDisplayOverrides());
                if (!dispOverridesStr.empty() && dispOverridesStr.back() == '\n') dispOverridesStr.pop_back();
                content.setDisplayOverrides(dispOverridesStr);
            }

            if (digitalContentDto.hasPublishUp()) {
                if (!digitalContentDto.getPublishUp().empty()) {
                    auto pubUp = parseDateTime(digitalContentDto.getPublishUp());
                    if (pubUp) {
                        content.setPublishUp(*pubUp);
                    } else {
                        content.setPublishUpToNull();
                    }
                } else {
                    content.setPublishUpToNull();
                }
            }

            if (digitalContentDto.hasPublishDown()) {
                if (!digitalContentDto.getPublishDown().empty()) {
                    auto pubDown = parseDateTime(digitalContentDto.getPublishDown());
                    if (pubDown) {
                        content.setPublishDown(*pubDown);
                    } else {
                        content.setPublishDownToNull();
                    }
                } else {
                    content.setPublishDownToNull();
                }
            }

            if (digitalContentDto.hasVersion()) {
                content.setVersion(digitalContentDto.getVersion());
            } else {
                content.setVersion(content.getValueOfVersion() + 1);
            }

            if (digitalContentDto.hasLanguage()) {
                content.setLanguage(digitalContentDto.getLanguage());
            }

            if (digitalContentDto.hasModifiedBy()) {
                if (!digitalContentDto.getModifiedBy().empty()) {
                    content.setModifiedBy(digitalContentDto.getModifiedBy());
                } else {
                    content.setModifiedByToNull();
                }
            }

            content.setModifiedAt(trantor::Date::now());

            if (digitalContentDto.hasViews()) {
                content.setViews(digitalContentDto.getViews());
            }

            co_await mp.update(content);

            response.success = true;
            response.message = "Digital content updated successfully";

        } catch (const UnexpectedRows &e) {
            response.success = false;
            response.message = "Digital content not found";
            response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to update digital content";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

    drogon::Task<dto::BaseApiResponse> DigitalContentService::deleteContent(const std::string &digitalContentId) {
        auto dbClient = drogon::app().getDbClient();
        auto mp = CoroMapper<DigitalContents>(dbClient);

        dto::BaseApiResponse response;
        try {
            auto count = co_await mp.deleteBy(Criteria(DigitalContents::Cols::_id, CompareOperator::EQ, digitalContentId));

            if (count > 0) {
                response.success = true;
                response.message = "Digital content deleted successfully";
            } else {
                response.success = false;
                response.message = "Digital content not found";
                response.error["code"] = constants::ERR_RESOURCE_NOT_FOUND;
            }

        } catch (const DrogonDbException &e) {
            response.success = false;
            response.message = "Failed to delete digital content";
            response.error["detail"] = e.base().what();
        }
        co_return response;
    }

}