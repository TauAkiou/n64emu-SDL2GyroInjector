//
// Created by robin on 1/20/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_INPUTHANDLER_H
#define INC_1964_INPUT_JOYSHOCKCPP_INPUTHANDLER_H

#define SMOOTHBUFLEN 16

#include <cmath>
#include <chrono>
#include "../common.h"
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
        float getStickLength(float stickX, float stickY);
        float WarpEaseOut(float input);
        float getDirectStickRotation(float input);
        float getSmoothedStickRotation(PLAYERS player, float input);
        float getTieredSmoothedStickRotation(PLAYERS player, float input, float threshold1, float threshold2);
        float zeroTurnSmoothing(PLAYERS player);
        float ClampFloat(float value, float min, float max);

    public:
        InputHandler() = default;
        vec2<float> ProcessAimStickInputForPlayer(PLAYERS player);
        vec2<float> HandleDeadZoneStickInput(vec2<float> stick, vec2<float> deadzone);




};

#endif //INC_1964_INPUT_JOYSHOCKCPP_INPUTHANDLER_H
