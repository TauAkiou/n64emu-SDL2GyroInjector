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
 * GameDriver.h - Base class for Game Drivers.
 */


#ifndef INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H
#define INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H

#include "../EmulatorLink.h"
#include "../common/Helpers.h"
#include "../settings/Settings.h"
#include "../../common/common.h"
#include "../plugin.h"
#include <string>

#define TICKRATE (_settings->GetIfEmulatorOverclocked() ? 2 : 4) // 2ms (500 Hz) for overclocked, 4ms (250 Hz) for stock speed
#define TIMESTEP TICKRATE / 1000

#ifdef SPEEDRUN_BUILD // fov/ratio hacks gives unfair advantage, always use default values for speedrun build
#define RATIOFACTOR 1.f
#define OVERRIDEFOV 60
#else
#define RATIOFACTOR (((float)_settings->GetOverrideRatioWidth() / (float)_settings->GetOverrideRatioHeight()) / (16.f / 9.f))
#define OVERRIDEFOV _settings->GetFovOverride()
#endif



class GameDriver {
protected:
    Settings* _settings = Settings::GetInstance();
    ControlState* _cfgptr = ControlState::GetInstance();
    EmulatorLink* _link = nullptr;
    std::string _name;
public:
    std::string GetName() {
        return std::string(_name);
    }
    virtual int Status() = 0;
    virtual void Inject() = 0;
    virtual void Quit() = 0;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H
