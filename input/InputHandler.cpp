//
// Created by robin on 1/20/2021.
//

#include "InputHandler.h"

vec2<float> InputHandler::ProcessAimStickInputForPlayer(PLAYERS player) {
    switch(_ctrlptr->Profile[player].SETTINGS[STICKMODE]) {
        default:
        case FULL:
            return { _ctrlptr->Device[player].AIMSTICKX, _ctrlptr->Device[player].AIMSTICKY };
        case XONLY:
            return { _ctrlptr->Device[player].AIMSTICKX, 0 };
        case FLICK:
            auto vec = getFlickState(player);
            laststick[player] = { _ctrlptr->Device[player].AIMSTICKX, _ctrlptr->Device[player].AIMSTICKY };
            return { vec, 0};
    }
}

float InputHandler::ClampFloat(const float value, const float min, const float max) {
    const float test = value < min ? min : value;
    return test > max ? max : test;
}

float InputHandler::getStickLength(float stickX, float stickY) {
    return sqrtf(stickX * stickX + stickY * stickY);
}

// Flick Stick and smoothing code adapted from JoyShockMapper.

float InputHandler::getFlickState(PLAYERS player) {
    float result = 0.0;

    float lastLength = getStickLength(laststick[player].x, laststick[player].y);
    float length = getStickLength(_ctrlptr->Device[player].AIMSTICKX, _ctrlptr->Device[player].AIMSTICKY);

    if(length >= flickthreshold) {
        float stickangle = atan2f(-_ctrlptr->Device[player].AIMSTICKX, _ctrlptr->Device[player].AIMSTICKY);

        if(!isflicking[player]) {
            isflicking[player] = true;


            }
        }
    }
    else {
        // turning cleanup code
        if (lastLength >= flickthreshold) {
            // we've just transitioned from flick/turn to no flick, so clean up
            zeroTurnSmoothing(player);
        }
    }

    float lastFlickProgress = flickprogress[player];
    if(lastFlickProgress < flicktime) {
        flickprogress[player] = std::min(flickprogress[player] + _ctrlptr->DeltaTime, flicktime);

        //
        float lastPerOne = lastFlickProgress / flicktime;
        float thisPerOne = flickprogress[player] / flicktime;

        float warpedLastPerOne = WarpEaseOut(lastPerOne);
        float warpedThisPerOne = WarpEaseOut(thisPerOne);

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

float InputHandler::getSmoothedStickRotation(PLAYERS player, float input) {
    bufferpos[player] = (bufferpos[player] + 1) % SMOOTHBUFLEN;
    aimstickbuffer[player][bufferpos[player]] = input;

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
        directWeight = ClampFloat(directWeight, 0.0, 1.0);

        return getDirectStickRotation(input * directWeight) +
               getSmoothedStickRotation(player, input * (1.0 - directWeight));
}

float InputHandler::zeroTurnSmoothing(PLAYERS player) {
    for(float fl : aimstickbuffer[player])
        fl = 0;
}
