cmake_minimum_required(VERSION 3.17)
project(1964_input_joyshockcpp)
set(CMAKE_CXX_STANDARD 17)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

include(FetchContent)

# automatically download nlohmann/json.

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

set(SDL2_DIR "C:\\msvc\\SDL2-2.0.16")
set(Qt5_DIR "C:\\msvc\\qt-5_15-x86-msvc2017-static\\lib\\cmake\\Qt5")

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
find_package(SDL2 REQUIRED STATIC)
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

# used for automatic deployment & debugging purposes.
SET(1964_INSTALL_DIRECTORY "C:/Games/1964")

SET(GCC_COVERAGE_COMPILE_FLAGS "/MTd")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

include_directories(${CMAKE_BINARY_DIR})
add_library(1964_input_joyshockcpp SHARED
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
        ui/ConfigDialog.cpp
        ui/ConfigDialog.h
        ui/1964_config.ui
        input/InputClasses.h
        input/InputClasses.cpp
        input/GamepadMotion/GamepadMotion.hpp
        input/GamepadMotion/GamepadMotion.cpp
        common/common.cpp)

if(MSVC)
    set("FORCE_STATIC_VCRT" "ON")
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    set(BUILD_SHARED_LIBS OFF)

    if(POLICY CMP0091)
        cmake_policy(SET CMP0091 NEW)
    endif()

    set_property(TARGET 1964_input_joyshockcpp PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # set_property(TARGET SDL2 PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif(MSVC)

target_link_directories(1964_input_joyshockcpp PRIVATE res)
target_link_libraries(1964_input_joyshockcpp PUBLIC Qt5::Widgets Qt5::Core Qt5::Gui)
target_link_libraries(1964_input_joyshockcpp PRIVATE nlohmann_json::nlohmann_json SDL2)


# Automatic deployment for the plugin.
IF(1964_INSTALL_DIRECTORY)
    message("${PROJECT_BINARY_DIR}")
    SET(1964_PLUGIN_DIRECTORY ${1964_INSTALL_DIRECTORY}/plugin)
    #add_custom_command(TARGET 1964_input_joyshockcpp POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove
    #${1964_PLUGIN_DIRECTORY}/)
    add_custom_command(TARGET 1964_input_joyshockcpp POST_BUILD VERBATIM
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_BINARY_DIR}/1964_input_joyshockcpp.dll"
            "${1964_PLUGIN_DIRECTORY}/1964_input_joyshockcpp.dll"
            COMMENT "Copying to 1964 Directory...")
ENDIF()