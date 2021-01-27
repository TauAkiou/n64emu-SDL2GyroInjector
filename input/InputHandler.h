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
 * InputHandler.h - Class for transforming and altering controller input based on control style.
 */


#ifndef INC_1964_INPUT_JOYSHOCKCPP_INPUTHANDLER_H
#define INC_1964_INPUT_JOYSHOCKCPP_INPUTHANDLER_H

#define SMOOTHBUFLEN 16

#include <cmath>
#include <chrono>
#include "../common/Helpers.h"
#include "../common/common.h"
#include "../settings/Settings.h"
#include "../common/vec2.h"

class InputHandler {
    private:
        ControlState* _ctrlptr = ControlState::GetInstance();
        Settings* _settings = Settings::GetInstance();

        const float flickthreshold = 0.9;
        const float flicktime = 0.1;
        const float turnsmooththreshold = 0.1;

        // Store flick state for up to 4 devices.
        float flickprogress[ALLPLAYERS] = { 0, 0, 0, 0 };
        float flicksize[ALLPLAYERS] = { 0, 0, 0, 0 };
        vec2<float> laststick[ALLPLAYERS];

        float aimstickbuffer[ALLPLAYERS][SMOOTHBUFLEN]{};
        int stickbufferpos[ALLPLAYERS] = { 0 };

        vec2<float> gyrobuffer[ALLPLAYERS][SMOOTHBUFLEN];
        int gyrobufferpos[ALLPLAYERS] = { 0 };

        float getFlickState(PLAYERS player, const vec2<float> &stick);
        static float getStickLength(float stickX, float stickY);
        static float WarpEaseOut(float input);
        static float getDirectStickRotation(float input);
        float getSmoothedStickRotation(PLAYERS player, float input);
        float getTieredSmoothedStickRotation(PLAYERS player, float input, float threshold1, float threshold2);
        void zeroTurnSmoothing(PLAYERS player);

    public:
        InputHandler() = default;
        vec2<float> ProcessAimStickInputForPlayer(PLAYERS player);
        static vec2<float> HandleDeadZoneStickInput(vec2<float> stick, vec2<float> deadzone);

    static vec2<float> GetBaseFactorForStickType(enum STICKMODE mode);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_INPUTHANDLER_H
