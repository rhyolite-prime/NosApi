cmake_minimum_required(VERSION 3.5)
project(NosApi CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fcoroutines)
endif()

add_executable(${PROJECT_NAME} main.cc
        services/folders/FolderService.h
        services/collections/CollectionService.h
        services/categories/CategoryService.h
        constants/StatusTypes.h
        services/fields/FieldService.h
        services/access_levels/AccessLevelService.h
        services/field_groups/FieldGroupService.h
        services/categories/CategoryService.cc
        services/access_levels/AccessLevelService.cc
        services/users/UserService.h
        services/users/UserService.cc
        services/user_groups/UserGroupService.h
        services/subscription_plans/SubscriptionPlanService.h
        services/roles/RoleService.h
        services/resources/ResourceService.h
        services/content/DigitalContentService.h
        services/user_groups/UserGroupService.cc
        services/work_flows/WorkflowService.h
        services/work_flows/WorkflowService.cc
        services/subscription_plans/SubscriptionPlanService.cc
        services/roles/RoleService.cc
        services/resources/ResourceService.cc
        services/folders/FolderService.cc
        services/fields/FieldService.cc
        services/field_groups/FieldGroupService.cc
        services/content/DigitalContentService.cc
        services/collections/CollectionService.cc
        filters/JwtAuthFilter.h
        dto/BaseApiResponse.h
        dto/SigninDto.h
        dto/UserPasskeyDto.h
        constants/ErrorCodes.h
        utils/PasswordUtils.h
        utils/PasswordUtils.cc
        dto/RegisterUserPasskeysDto.h
        dto/RoleDto.h
        dto/UserGroupDto.h
        dto/UserDto.h
        dto/AccessLevelDto.h
        dto/WorkflowDto.h
        dto/WorkflowStageDto.h
        dto/WorkflowTransitionDto.h
        services/email/EmailService.h
        services/email/EmailService.cc
        dto/DigitalContentDto.h
        services/content_types/ContentTypeService.h
        services/content_types/ContentTypeService.cc
        dto/ContentTypeDto.h
        dto/CategoryDto.h
        dto/SendEmailDto.h)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# add_subdirectory(drogon) 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
find_package(Drogon CONFIG REQUIRED)

find_package(OpenSSL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Drogon::Drogon
        bcrypt
        jwt-cpp
        OpenSSL::SSL
        OpenSSL::Crypto
)

# ##############################################################################

if (CMAKE_CXX_STANDARD LESS 17)
    message(FATAL_ERROR "c++17 or higher is required")
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
#aux_source_directory(utils UTILS_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)
# use the following line to create views with namespace CHANGE_ME prefixed
# and path namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE CHANGE_ME)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external_deps/jwt-cpp/include
        ${CMAKE_CURRENT_SOURCE_DIR}
       ${CMAKE_CURRENT_SOURCE_DIR}/models)


target_sources(${PROJECT_NAME}
               PRIVATE
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC})

target_compile_options(${PROJECT_NAME} PRIVATE "-include" "${CMAKE_CURRENT_SOURCE_DIR}/utils/codecvt_helper.h" "-Wno-deprecated-declarations")

# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

add_subdirectory(test)
add_subdirectory(external_deps/Bcrypt)
add_subdirectory(external_deps/jwt-cpp)
