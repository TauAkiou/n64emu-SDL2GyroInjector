/*
 *==========================================================================
 * Joyshock Injector - 1964 Plugin
 *==========================================================================
 * Largely based off of the original Keyboard/Mouse Injector Plugin
 * Copyright (C) 2016-2021 Carnivorous
 *
 * Joyshock Injector
 * Copyright (C) 2016-2021 TauAkiou
 * All rights reserved.
 *
 * Joyshock Injector is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit http://www.gnu.org/licenses/gpl-2.0.html
 *==========================================================================
 */

/*
 * Settings.h - Classes for storing configuration values & state.
 */


#ifndef INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H
#define INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H

#include "../common/common.h"
#include "../plugin.h"

class Settings {
private:
    static inline Settings* _instance;

    PROFILE Profile[4] {};
    Assignment ControllerAssignments[4] = {};

    // Plugin/Emulator settings
    bool EmulatorOverclocked = true;
    int FovOverride = 60;
    int OverrideRatioWidth = 16;
    int OverrideRatioHeight = 9;
    bool ShowGoldeneyeCrosshair = false;

public:
    static Settings* GetInstance();
    PROFILE GetProfileForPlayer(enum PLAYERS player);
    bool GetIfPlayerIsConnected(enum PLAYERS player);
    Assignment GetAssignmentForPlayer(enum PLAYERS player);
    void SetAssignmentForPlayer(enum PLAYERS player, Assignment asgn);
    [[nodiscard]] bool GetIfEmulatorOverclocked() const;
    void SetIfEmulatorOverclocked(bool val);
    int GetFovOverride() const;
    void SetFovOverride(int fov);
    int GetOverrideRatioWidth() const;
    [[nodiscard]] int GetOverrideRatioHeight() const;
    [[nodiscard]] bool GetShowGoldeneyeCrosshair() const;
    void SetProfileForPlayer(enum PLAYERS player, PROFILE profile);
    void SetShowGoldeneyeCrosshair(bool val);
};

/*
 * ControlState:
 *
 * Stores input working state.
 *
 */

class ControlState {
private:
    inline static ControlState* _instance;
    ControlState() = default;
public:
    static ControlState* GetInstance();
    DEVICE Device[ALLPLAYERS] = {};
    float DeltaTime = 0.0;
};
#endif //INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H