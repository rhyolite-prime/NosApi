/**
 *
 *  NosApiPlugin.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>

#include "services/access_levels/AccessLevelService.h"
#include "services/categories/CategoryService.h"
#include "services/content/DigitalContentService.h"
#include "services/content_types/ContentTypeService.h"
#include "services/email/EmailService.h"
#include "services/roles/RoleService.h"
#include "services/users/UserService.h"
#include "services/user_groups/UserGroupService.h"
#include "services/work_flows/WorkflowService.h"

namespace nos::plugins {

    class NosApiPlugin : public drogon::Plugin<NosApiPlugin>
    {
    public:

        NosApiPlugin() = default;
        ~NosApiPlugin() override = default;
        /// This method must be called by drogon to initialize and start the plugin.
        /// It must be implemented by the user.
        void initAndStart(const Json::Value &config) override;

        /// This method must be called by drogon to shutdown the plugin.
        /// It must be implemented by the user.
        void shutdown() override;

        ::nos::services::RoleService &getRoleService() { return roleService_; }
        ::nos::services::UserService &getUserService() { return userService_; }
        ::nos::services::UserGroupService &getUserGroupService() { return userGroupService_; }
        ::nos::services::AccessLevelService &getAccessLevelService() { return accessLevelService_; }
        ::nos::services::WorkflowService &getWorkflowService() { return workflowService_; }
        ::nos::services::EmailService &getEmailService() { return emailService_; }
        ::nos::services::ContentTypeService &getContentTypeService() { return contentTypeService_; }
        ::nos::services::CategoryService &getCategoryService() { return categoryService_; }
        ::nos::services::DigitalContentService &getDigitalContentService() { return digitalContentService_; }

    private:
        ::nos::services::RoleService roleService_;
        ::nos::services::UserService userService_;
        ::nos::services::UserGroupService userGroupService_;
        ::nos::services::AccessLevelService accessLevelService_;
        ::nos::services::WorkflowService workflowService_;
        ::nos::services::EmailService emailService_;
        ::nos::services::ContentTypeService contentTypeService_;
        ::nos::services::CategoryService categoryService_;
        ::nos::services::DigitalContentService digitalContentService_;

    };

}
