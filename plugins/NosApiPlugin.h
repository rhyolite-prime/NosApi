/**
 *
 *  NosApiPlugin.h
 *
 */

#pragma once

#include <drogon/plugins/Plugin.h>
#include "services/users/UserService.h"
#include "services/user_groups/UserGroupService.h"

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

        ::nos::services::UserService &getUserService() { return userService_; }
        ::nos::services::UserGroupService &getUserGroupService() { return userGroupService_; }

    private:
        ::nos::services::UserService userService_;
        ::nos::services::UserGroupService userGroupService_;

    };

}
