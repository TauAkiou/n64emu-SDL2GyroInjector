//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H
#define INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H

#include "../common.h"

class Controls {
    private:
        static Controls* instance;
        Controls();
    public:
        static Controls* getInstance();
        PROFILE Profile[4];
        DEVICE Device[4];
        float DeltaTime;
};

class PluginSettings {
private:
    static PluginSettings* _instance;
    PluginSettings();
public:
    static PluginSettings* getInstance();
    bool EmulatorOverclocked = true;
    int _fovoverride = 60;
    int _overrideratiowidth = 16;
    int _overrideratioheight = 9;
    bool _showgoldeneyecrosshair = false;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H