cmake_minimum_required(VERSION 3.17)
project(1964_input_joyshockcpp)

if(MINGW)
    set(CMAKE_RC_COMPILER_INIT windres)
    ENABLE_LANGUAGE(RC)
    SET(CMAKE_RC_COMPILE_OBJECT
            "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.12.10\\5.12.10\\mingw73_32")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets REQUIRED)

# used for automatic deployment & debugging purposes.
SET(1964_INSTALL_DIRECTORY "C:/Games/1964")
SET(1964_PLUGIN_DIRECTORY ${1964_INSTALL_DIRECTORY}/plugin)

SET(GCC_COVERAGE_COMPILE_FLAGS "-m32 -Wall")
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_BINARY_DIR})
add_library(1964_input_joyshockcpp SHARED
        maindll.cpp
        maindll.h
        plugin.h
        input/JoyShockDriver.cpp
        input/JoyShockDriver.h
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
        JoyShockLibrary/JoyShockLibrary.h
        input/InputHandler.h
        input/InputHandler.cpp
        common/Helpers.cpp
        common/Helpers.h
        1964_plugin.cpp
        1964_plugin.h
        common/vec2.h
        ui/resource.h
        ui/ConfigDialog.cpp
        ui/ConfigDialog.h
        ui/1964_config.ui
        ui/MappingButton.h
        )

target_include_directories(1964_input_joyshockcpp PUBLIC JoyShockLibrary/)
target_link_directories(1964_input_joyshockcpp PUBLIC JoyShockLibrary/)
target_link_libraries(1964_input_joyshockcpp JoyShockLibrary Qt5::Widgets)


add_custom_command(TARGET 1964_input_joyshockcpp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_CURRENT_BINARY_DIR}/lib1964_input_joyshockcpp.dll
         ${1964_PLUGIN_DIRECTORY}/)