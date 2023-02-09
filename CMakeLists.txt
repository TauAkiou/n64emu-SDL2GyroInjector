cmake_minimum_required(VERSION 3.17)
project(SDL2_GyroInjector)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_BINARY_DIR})

# automatically download nlohmann/json.
message("Fetching nhlohmann/json...")
include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_TAG v3.10.4)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_MakeAvailable(json)

# set up directories for SDL2 and Qt5, other required packages.

IF(NOT DEFINED SDL2_DIR)
    message(FATAL_ERROR "SDL2 Directory is not defined. Please make sure you have defined the SDL2 path in the cmake command line.")
endif()

if(NOT DEFINED Qt5_DIR)
    message(FATAL_ERROR "Qt5 Directory is not defined. Please make sure you have defined the SDL2 path in the cmake command line.")
endif()

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# compiler specific settings

if(MINGW)
    set(CMAKE_RC_COMPILER_INIT windres)
    ENABLE_LANGUAGE(RC)
    SET(CMAKE_RC_COMPILE_OBJECT
            "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_BINARY_DIR})
add_library(SDL2_GyroInjector SHARED
        maindll.cpp
        maindll.h
        plugin.h
        input/SdlDriver.cpp
        input/SdlDriver.h
        common/common.h
        game/EmulatorLink.cpp
        game/EmulatorLink.h
        game/Game.cpp
        game/Game.h
        game/driver/PerfectDark.cpp
        game/driver/Goldeneye.cpp
        game/driver/Goldeneye.h
        game/driver/GameDriver.h
        game/driver/PerfectDark.h
        settings/Settings.cpp
        settings/Settings.h
        input/InputHandler.h
        input/InputHandler.cpp
        common/Helpers.cpp
        common/Helpers.h
        1964_plugin.cpp
        1964_plugin.h
        common/vec2.h
        common/vec3.h
        ui/ConfigDialog.cpp
        ui/ConfigDialog.h
        ui/1964_config.ui
        input/SDLDevice.h
        input/SDLDevice.cpp
        input/GamepadMotion/GamepadMotion.hpp
        input/GamepadMotion/GamepadMotion.cpp
        common/common.cpp inj_version.h.in)

if(MSVC)
    set("FORCE_STATIC_VCRT" "ON")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    set(BUILD_SHARED_LIBS OFF)

    if(POLICY CMP0091)
        cmake_policy(SET CMP0091 NEW)
    endif()

    set_property(TARGET SDL2_GyroInjector PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # set_property(TARGET SDL2 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif(MSVC)

message("${SDL2_LIBRARIES}")

#target_link_directories(SDL2_GyroInjector PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(SDL2_GyroInjector PUBLIC Qt5::Widgets Qt5::Core Qt5::Gui ${SDL2_LIBRARIES})
target_link_libraries(SDL2_GyroInjector PRIVATE nlohmann_json::nlohmann_json)

if(GITRELEASE)
# if we are doing a git release, then we will load the


ENDIF()

# Automatic deployment for the plugin.
IF(1964_INSTALL_DIRECTORY)
    SET(1964_PLUGIN_DIRECTORY ${1964_INSTALL_DIRECTORY}/plugin)
    #add_custom_command(TARGET SDL2_GyroInjector POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove
    #${1964_PLUGIN_DIRECTORY}/)
    add_custom_command(TARGET SDL2_GyroInjector POST_BUILD VERBATIM
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_BINARY_DIR}/SDL2_GyroInjector.dll"
            "${1964_PLUGIN_DIRECTORY}/SDL2_GyroInjector.dll"
            COMMENT "Copying to 1964 Directory...")
ENDIF()

# version code obtained from: https://github.com/nocnokneo/cmake-git-versioning-example
# license information can be found under version.LICENSE
find_package(Git)
add_custom_target(version
        ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/inj_version.h.in
        -D DST=${CMAKE_BINARY_DIR}/inj_version.h
        -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
        -P ${CMAKE_SOURCE_DIR}/GenerateVersionHeader.cmake
        )

add_dependencies(SDL2_GyroInjector version)