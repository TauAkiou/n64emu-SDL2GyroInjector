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
 * PerfectDark.h - Class for injecting controller input into Perfect Dark.
 */


#ifndef INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H
#define INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H

#include "../../input/InputHandler.h"
#include "../../common/Helpers.h"
#include "GameDriver.h"

#define PD_ResetCrouchToggle(X) safetocrouch[X] = 1, safetostand[X] = 0, crouchstance[X] = 2 // reset crouch toggle bind
#define PD_ResetXYStick(X) xstick[X] = 0, ystick[X] = 0, usingstick[X] = 0 // reset x/y stick array
#define PD_ResetRadialMenuBtns(X) for(int direction = 0; direction < 4; direction++) radialmenudirection[X][direction] = 0 // reset direction buttons
#define PD_WITHINRANGE(param) ((_link.rdramptr != 0) && ((param & 0xFF800000U) == 0x80000000U))


class PerfectDark : public GameDriver {
private:
    std::string _name = "Perfect Dark";
    unsigned int playerbase[4] = {0};
    InputHandler _ihandler;
    int xstick[4] = {0};
    int ystick[4] = {0};
    int usingstick[4] = {0};
    int xmenu[4] = {0};
    int ymenu[4] = {0};
    int radialmenudirection[4][4] = {{0}, {0}, {0}, {0}};
    int safetocrouch[4] = { 1, 1, 1, 1 };
    int safetostand[4] = {0};
    int crouchstance[4] = {0};
    float crosshairposx[4]{};
    float crosshairposy[4]{};
    float aimx[4]{};
    float aimy[4]{};
    int gunrcenter[4]{};
    int gunlcenter[4]{};

    void _crouch(int player, const js_settings::PROFILE& profile);
    void _camspyslayer(int player, const js_settings::PROFILE &profile, int camspyflag, float sensitivityx, float sensitivityy);
    void _processOriginalInput(int player, const js_settings::PROFILE& profile);
    void _aimmode(int player, const js_settings::PROFILE& profile, int aimingflag, float fov, float basefov);
    void _radialmenunav(int player, const js_settings::PROFILE& profile);
    void _resetgyro();
    void _controller();
    void _injecthacks();
    void _processFreeAimInput(int player, const js_settings::PROFILE& profile);
    void _aimmode_free(const int player, const js_settings::PROFILE& profile, const int aimingflag, const float fov, const float basefov);

public:
    explicit PerfectDark(EmulatorLink* linkptr);
    int Status() override;
    void Inject() override;
    void Quit() override;

    void _processAnalogMovement(const int player, const js_settings::PROFILE &profile);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H
