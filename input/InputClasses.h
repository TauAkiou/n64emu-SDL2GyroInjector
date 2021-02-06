//
// Created by Robin on 2/4/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_INPUTCLASSES_H
#define INC_1964_INPUT_JOYSHOCKCPP_INPUTCLASSES_H

#include <SDL_gamecontroller.h>
#include <SDL_events.h>
#include <string>
#include <memory>
#include <limits>
#include <iostream>
#include "../common/common.h"
#include "../common/Helpers.h"

typedef struct {
    float GyroX = 0.0f;
    float GyroY = 0.0f;
    float GyroZ = 0.0f;
    float AccelX = 0.0f;
    float AccelY = 0.0f;
    float AccelZ = 0.0f;
} MotionReport;

typedef struct {
    vec2<float> LStick { 0, 0 };
    vec2<float> RStick {0, 0 };
    float LTrigger = 0;
    float RTrigger = 0;
} AxisReport;

class SDLDevice {
    private:
        SDL_GameController* _sdlgcptr = nullptr;
        std::string _controllerName;
        SDL_GameControllerType _controllerType;

        bool _deviceHasGyroscope = false;
        bool _deviceHasAccelerometer = false;
        bool _deviceHasPaddles = false;
        int _colorbar_color = 0x000000;
        int _sdlplayerindex = -1;

    public:
        SDLDevice(int bindindex);
        ~SDLDevice();
        std::string GetControllerName();
        std::string GetButtonNameForDevice(SDL_GameControllerButton button);
        std::string GetAxisNameForDevice(SDL_GameControllerAxis axis);
        std::string GetButtonNameFromBitmask(int mask);
        MotionReport GetCurrentMotionReport();
        AxisReport GetCurrentAxisReport();
        unsigned int GetCurrentButtonState();
        void AssignPlayerIndex(int index);
        int GetFirstPressedButton();

};

typedef struct assgn {
    enum CONTROLLERMODE ControllerMode = DISCONNECTED;
    std::shared_ptr<SDLDevice> PrimaryDevice;
    std::shared_ptr<SDLDevice> SecondaryDevice;
} Assignment;

#endif //INC_1964_INPUT_JOYSHOCKCPP_INPUTCLASSES_H
