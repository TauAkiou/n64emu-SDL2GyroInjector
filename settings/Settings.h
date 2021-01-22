//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H
#define INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H

#include "../common.h"
#include "../plugin.h"

class Settings {
private:
    static inline Settings* _instance;
    public:
    static Settings* GetInstance();
       PROFILE Profile[4] { 0,
                             { -1, None, 0, 0},
                             {-1, None, 0, 0} };

       // Plugin/Emulator settings
    bool EmulatorOverclocked = true;
    int FovOverride = 60;
    int OverrideRatioWidth = 16;
    int OverrideRatioHeight = 9;
    bool ShowGoldeneyeCrosshair = false;
};

/*
 * ControlState:
 *
 * Stores input working state.
 *
 */

class ControlState {
private:
    static ControlState* _instance;
    ControlState() = default;
public:
    static ControlState* GetInstance();
    DEVICE Device[ALLPLAYERS] = {};
    float DeltaTime = 0.0;
};
#endif //INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H