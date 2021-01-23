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


#include <iostream>
#include "InputHandler.h"

vec2<float> InputHandler::ProcessAimStickInputForPlayer(PLAYERS player) {
    vec2<float> vec;
    auto profile = _settings->GetProfileForPlayer(player);
    switch(profile.StickMode) {
        default:
        case FULLSTICK:
            return HandleDeadZoneStickInput(_ctrlptr->Device[player].AIMSTICK,
                                            profile.AimstickDeadzone);
        case XONLY:
            vec = HandleDeadZoneStickInput(_ctrlptr->Device[player].AIMSTICK,
                                                profile.AimstickDeadzone);
            // Zero the y.
            vec.y = 0;
            return vec;
        case FLICK: // Ignore deadzone calculations for flickstick, for the moment.
            auto flick = getFlickState(player, _ctrlptr->Device[player].AIMSTICK);
            laststick[player] = _ctrlptr->Device[player].AIMSTICK;
            return { flick, 0 };
    }
}

// Flick Stick and smoothing code adapted from http://gyrowiki.jibbsmart.com/blog:good-gyro-controls-part-2:the-flick-stick
// Very special thanks to Jibb Smart for sharing his implementation!

float InputHandler::getFlickState(PLAYERS player, const vec2<float> &stick) {
    float result = 0.0;

    float lastLength = getStickLength(laststick[player].x, laststick[player].y);
    float length = getStickLength(stick.x, stick.y);

    if(length >= flickthreshold) {
        if (lastLength < flickthreshold) {
            float stickangle = atan2f(stick.x, stick.y);

            // We have a new flick.
            flickprogress[player] = 0;
            flicksize[player] = atan2f(stick.x, -stick.y);

            //std::cout << "Flick " << stickangle * (180.0f / (float) PI) << std::endl;
        }
        else {
            float stickAngle = atan2f(stick.x, -stick.y);
            float lastStickAngle = atan2f(laststick[player].x, -laststick[player].y);
            float angleChange = stickAngle - lastStickAngle;
            // https://stackoverflow.com/a/11498248/1130520
            angleChange = fmod(angleChange + PI, 2.0f * PI);
            if (angleChange < 0)
                angleChange += 2.0f * PI;
            angleChange -= PI;
            result += getTieredSmoothedStickRotation(player, angleChange,
                                                     turnsmooththreshold / 2.0f, turnsmooththreshold);
        }
    }
    else {
        // turn cleanup
        if (lastLength >= flickthreshold) {
            // we've just transitioned from flick/turn to no flick, so clean up
            zeroTurnSmoothing(player);
        }
    }


        float lastFlickProgress = flickprogress[player];
        if (lastFlickProgress < flicktime) {
            flickprogress[player] = std::min(flickprogress[player] + _ctrlptr->DeltaTime, flicktime);

            // get last time and this time in 0-1 completion range
            float lastPerOne = lastFlickProgress / flicktime;
            float thisPerOne = flickprogress[player] / flicktime;

            // our WarpEaseOut function stays within the 0-1 range but pushes it all closer to 1
            float warpedLastPerOne = WarpEaseOut(lastPerOne);
            float warpedThisPerOne = WarpEaseOut(thisPerOne);

            // now use the difference between last frame/sample and this frame/sample
            result += (warpedThisPerOne - warpedLastPerOne) * flicksize[player];
        }

    return result;
}

float InputHandler::WarpEaseOut(float input) {
    float flipped = 1.0f - input;
    return 1.0f - flipped * flipped;
}

float InputHandler::getDirectStickRotation(float input) {
    return input;
}

vec2<float> InputHandler::HandleDeadZoneStickInput(const vec2<float> stick, const vec2<float> deadzone) {
    auto vector = vec2<float>(0,0);
    if(stick.x > deadzone.x || stick.x < -deadzone.x) {
        vector.x = stick.x;
    }
    if(stick.y > deadzone.y || stick.y < -deadzone.y) {
        vector.y = stick.y;
    }

    return vector;

}

float InputHandler::getSmoothedStickRotation(PLAYERS player, float input) {
    stickbufferpos[player] = (stickbufferpos[player] + 1) % SMOOTHBUFLEN;
    aimstickbuffer[player][stickbufferpos[player]] = input;

    float average = 0.0;
    for(float sample : aimstickbuffer[player]) {
        average += sample;
    }
    average /= SMOOTHBUFLEN;

    return average;
}

float InputHandler::getTieredSmoothedStickRotation(PLAYERS player, float input, float threshold1, float threshold2) {

        float inputMagnitude = fabsf(input);

        float directWeight = (inputMagnitude - threshold1) /
                             (threshold2 - threshold1);
        directWeight = PluginHelpers::ClampFloat(directWeight, 0.0, 1.0);

        return getDirectStickRotation(input * directWeight) +
               getSmoothedStickRotation(player, input * (1.0 - directWeight));
}

void InputHandler::zeroTurnSmoothing(PLAYERS player) {
    for(float fl : aimstickbuffer[player])
        fl = 0;
}

float InputHandler::getStickLength(float stickX, float stickY) {
    return sqrtf(stickX * stickX + stickY * stickY);
}
