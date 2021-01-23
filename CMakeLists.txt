cmake_minimum_required(VERSION 3.17)
project(1964_input_joyshockcpp)

SET(GCC_COVERAGE_COMPILE_FLAGS "-m32 -Wall")
SET(1964_INSTALL_DIRECTORY "C:/Games/1964")
SET(1964_PLUGIN_DIRECTORY ${1964_INSTALL_DIRECTORY}/plugin)

set(CMAKE_CXX_STANDARD 20)

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
        JoyShockLibrary/JoyShockLibrary.h input/InputHandler.h input/InputHandler.cpp common/Helpers.cpp common/Helpers.h 1964_plugin.cpp 1964_plugin.h common/vec2.h)

target_include_directories(1964_input_joyshockcpp PUBLIC JoyShockLibrary/)
target_link_directories(1964_input_joyshockcpp PUBLIC JoyShockLibrary/)
target_link_libraries(1964_input_joyshockcpp JoyShockLibrary)


add_custom_command(TARGET 1964_input_joyshockcpp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/lib1964_input_joyshockcpp.dll
        ${1964_PLUGIN_DIRECTORY}/)