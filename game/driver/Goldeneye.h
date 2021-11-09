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
 * Goldeneye.h - Class for injecting controller input into Goldeneye 007.
 */

#ifndef INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H
#define INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H

#include "GameDriver.h"
#include "../../common/Helpers.h"
#include "../../common/vec2.h"
#include "../../input/InputHandler.h"

class Goldeneye : public GameDriver {
private:
    std::string _name = "GoldenEye 007";
    unsigned int playerbase[4] = {0};
    InputHandler _ihandler;
    int safetocrouch[4] = { 1, 1, 1, 1 };
    int safetostand[4] = {0};
    int crouchstance[4] = {0};
    float crosshairposx[4]{};
    float crosshairposy[4]{};
    float aimx[4]{};
    float aimy[4]{};
    void _crouch(const int player, const js_settings::PROFILE& profile);
    void _aimmode(const int player, const js_settings::PROFILE& profile, const int aimingflag, const float fov, const float basefov);
    void _resetgyro();
    void _controller();
    void _injecthacks();
    void _processFreeAim(int player, const js_settings::PROFILE& profile);
    void _aimmode_freeaim(const int player, const js_settings::PROFILE& profile, const int aimingflag, const float fov, const float basefov);
    void _processOriginalAimmode(int player, const js_settings::PROFILE& profile);
    void _processMenu(int player, const js_settings::PROFILE& profile);

public:
    explicit Goldeneye(EmulatorLink *linkptr);
    int Status() override;
    void Inject() override;
    void Quit() override;



};

#endif //INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H
