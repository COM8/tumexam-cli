cmake_minimum_required(VERSION 3.20)

set(PROJECT_DESCRIPTION "Client for https://tumexam.in.tum.de to unlock your device for printing.")
project("tumexam-cli" 
            VERSION 1.7.0.0
            DESCRIPTION "${PROJECT_DESCRIPTION}"
            HOMEPAGE_URL "https://github.com/COM8/tumexam-cli")
set(VERSION_NAME "dev")

set(CMAKE_CXX_STANDARD 20)

macro(tumexam_cli_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

macro(tumexam_cli_log_level OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    set(${OPTION_NAME} ${OPTION_DEFAULT} CACHE STRING ${OPTION_TEXT})
    set_property(CACHE ${OPTION_NAME} PROPERTY STRINGS "Default" "Trace" "Debug" "Info" "Warn" "Error" "Critical" "Off")
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

message(STATUS "TUMexam client options")
message(STATUS "=======================================================")
tumexam_cli_option(TUMEXAM_CLI_STATIC_ANALYZE "Set to ON to enable the GCC 10 static analysis. If enabled, TUMEXAM_CLI_ENABLE_LINTING has to be disabled." OFF)
tumexam_cli_option(TUMEXAM_CLI_ENABLE_LINTING "Set to ON to enable clang linting. If enabled, TUMEXAM_CLI_STATIC_ANALYZE has to be disabled." OFF)
tumexam_cli_log_level(TUMEXAM_CLI_LOG_LEVEL "Sets the log level. 'Default' will set the log level to 'Debug' for debug builds and 'Info' for release builds." "Off")
message(STATUS "=======================================================")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(sanitizer)
include(gcc_analyze)
include(clear_variable)
include(FetchContent)

if(TUMEXAM_CLI_ENABLE_LINTING)
    message(STATUS "Enabling linting")
    include(clang-tidy)
else()
    message(STATUS "Linting is disabled")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

# gtk4 stuff
include(glib-resource-compiler)
find_package(PkgConfig REQUIRED)
# Additional search path since gtkmm-4.0 will usually be installed in /usr
set(ENV{PKG_CONFIG_PATH} "/usr/lib/pkgconfig")
pkg_check_modules(GTKMM gtkmm-4.0>=4.4.0 REQUIRED IMPORTED_TARGET)

# Lib adwaita
pkg_check_modules(ADWAITA REQUIRED libadwaita-1>=1.1.0)
include_directories(${ADWAITA_INCLUDE_DIRS})

# Disable linting for fetch content projects
clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG db351ffbbadc6c4e9239daaa26e9aefa9f0ec82d) # The commit hash for 1.8.3. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git
                          GIT_TAG v3.10.5)
FetchContent_MakeAvailable(json)

set(SPDLOG_INSTALL ON)
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git
                            GIT_TAG v1.9.2)
FetchContent_MakeAvailable(spdlog)

restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

include_directories(${CMAKE_SOURCE_DIR}/src)
add_subdirectory(src)
