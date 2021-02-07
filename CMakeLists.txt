cmake_minimum_required(VERSION 3.17)
project(1964_input_joyshockcpp)

if(MINGW)
    set(CMAKE_RC_COMPILER_INIT windres)
    ENABLE_LANGUAGE(RC)
    SET(CMAKE_RC_COMPILE_OBJECT
            "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

if(QT_INSTALL)
    set(CMAKE_PREFIX_PATH ${QT_INSTALL})
else()
    message(FATAL_ERROR "The QT_INSTALL path must point to a QT directory.")
endif(QT_INSTALL)

if(!SDL_INCLUDE)
    message(FATAL_ERROR "SDL_INCLUDE must be set and point to your SDL '/include' directory.")
endif()



set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

# used for automatic deployment & debugging purposes.
SET(1964_INSTALL_DIRECTORY "C:/Games/1964 - sdl")

SET(GCC_COVERAGE_COMPILE_FLAGS "-m32 -Wall -O2")
set(CMAKE_CXX_STANDARD 20)

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
        input/GamepadMotion/GamepadMotion.hpp input/GamepadMotion/GamepadMotion.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${SDL_INCLUDE})
target_link_directories(1964_input_joyshockcpp PRIVATE res)

target_link_libraries(1964_input_joyshockcpp PUBLIC Qt5::Widgets Qt5::Core Qt5::Gui SDL2)
#target_link_libraries(1964_input_joyshockcpp PRIVATE Ws2_32.lib Version.lib imm32.lib winmm.lib)

# Automatic deployment for the plugin.
IF(1964_PLUGIN_DIRECTORY)
    SET(1964_PLUGIN_DIRECTORY ${1964_INSTALL_DIRECTORY}/plugin)
    add_custom_command(TARGET 1964_input_joyshockcpp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/lib1964_input_joyshockcpp.dll
        ${1964_PLUGIN_DIRECTORY}/)
ENDIF()