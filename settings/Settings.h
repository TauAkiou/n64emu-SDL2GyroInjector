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
        PROFILE Profile[4] { 0,
                             { -1, None, 0, 0},
                             {-1, None, 0, 0},
                             {0, 0},
                             {0, 0},
                             {0} };
        DEVICE Device[4] { 0, 0, 0, 0, 0, 0, 0, 0, {0}, {0}, {0}, false };
        float DeltaTime = 0;
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