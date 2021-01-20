//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H
#define INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H

#include "EmulatorLink.h"
#include "../settings/Settings.h"
#include "../common.h"
#include "../plugin.h"
#include <string>

#define TICKRATE (_pluginptr->EmulatorOverclocked ? 2 : 4) // 2ms (500 Hz) for overclocked, 4ms (250 Hz) for stock speed
#define TIMESTEP TICKRATE / 1000

#ifdef SPEEDRUN_BUILD // fov/ratio hacks gives unfair advantage, always use default values for speedrun build
#define RATIOFACTOR 1.f
#define OVERRIDEFOV 60
#else
#define RATIOFACTOR (((float)_pluginptr->OverrideRatioWidth / (float)_pluginptr->OverrideRatioHeight) / (16.f / 9.f))
#define OVERRIDEFOV _pluginptr->FovOverride
#endif



class GameDriver {
protected:
    GameDriver* _driver = nullptr;
    Controls* _cfgptr = nullptr;
    PluginSettings* _pluginptr = nullptr;
    EmulatorLink* _link = EmulatorLink::getInstance();
    std::string _name;
    float ClampFloat(const float value, const float min, const float max);
    int ClampInt(const int value, const int min, const int max);
public:
    std::string GetName() {
        return std::string(_name);
    }
    virtual int Status() = 0;
    virtual void Inject() = 0;
    virtual void Quit() = 0;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H
