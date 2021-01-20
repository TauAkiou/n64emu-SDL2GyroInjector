//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H
#define INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H

#include "../common.h"
#include "../plugin.h"

class Controls {
    private:
        static Controls* _instance;
        Controls() = default;
    public:
        static Controls* getInstance();
        PROFILE Profile[4];
        DEVICE Device[4];
        float DeltaTime;
        BUTTONS Controllers[4];
};

class PluginSettings {
private:
    static PluginSettings* _instance;
    PluginSettings() = default;
public:
    static PluginSettings* getInstance();
    bool EmulatorOverclocked = true;
    int FovOverride = 60;
    int OverrideRatioWidth = 16;
    int OverrideRatioHeight = 9;
    bool ShowGoldeneyeCrosshair = false;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H