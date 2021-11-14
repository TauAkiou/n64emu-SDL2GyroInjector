//
// Created by Robin on 2/4/2021.
//

#include "InputClasses.h"

SDLDevice::SDLDevice(int bindindex) {
        auto test =  SDL_GameControllerTypeForIndex(bindindex);
        if(!SDL_GameControllerTypeForIndex(bindindex)) {
            throw std::exception();
        }

        _sdlgcptr = SDL_GameControllerOpen(bindindex);

        if(_sdlgcptr == nullptr) {
            throw std::exception();
        }

        // Turn off events. Our event loop will handle polling.
        // TODO: Reconsider whether or not we should rework the event loop.
        SDL_GameControllerEventState(SDL_IGNORE);
        _controllerName = std::string(SDL_GameControllerName(_sdlgcptr));
        _controllerType = SDL_GameControllerGetType(_sdlgcptr);


    // Check for and enable Accelerometer and Gyroscope if we have them.
    _deviceHasAccelerometer = SDL_GameControllerHasSensor(_sdlgcptr, SDL_SENSOR_ACCEL);
    _deviceHasGyroscope = SDL_GameControllerHasSensor(_sdlgcptr, SDL_SENSOR_GYRO);

    if(_deviceHasAccelerometer) {
        SDL_GameControllerSetSensorEnabled(_sdlgcptr, SDL_SENSOR_ACCEL, SDL_TRUE);
    }
    if(_deviceHasGyroscope) {
        SDL_GameControllerSetSensorEnabled(_sdlgcptr, SDL_SENSOR_GYRO, SDL_TRUE);

    }
}

SDLDevice::~SDLDevice() {
    // When destroying the SDLDevice object, make sure that the device is closed.
    SDL_GameControllerClose(_sdlgcptr);
}

std::string SDLDevice::GetGenericAxisNames(SDL_GameControllerAxis axis) {
        switch(axis) {
            case SDL_CONTROLLER_AXIS_INVALID:
                return "Invalid";
            case SDL_CONTROLLER_AXIS_LEFTX:
                return "Left X-Axis";
            case SDL_CONTROLLER_AXIS_LEFTY:
                return "Left Y-Axis";
            case SDL_CONTROLLER_AXIS_RIGHTX:
                return "Right X-Axis";
            case SDL_CONTROLLER_AXIS_RIGHTY:
                return "Right Y-Axis";
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                return "LT";
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                return "RT";
            case SDL_CONTROLLER_AXIS_MAX:
            default:
                return "Unknown";
        }
}

std::string SDLDevice::GetAxisNameForDevice(SDL_GameControllerAxis axis) {
    switch(_controllerType) {

        case SDL_CONTROLLER_TYPE_UNKNOWN:
            return "Unknown";
        case SDL_CONTROLLER_TYPE_XBOX360:
        case SDL_CONTROLLER_TYPE_XBOXONE:
        case SDL_CONTROLLER_TYPE_VIRTUAL:
            switch(axis) {
                case SDL_CONTROLLER_AXIS_INVALID:
                    return "Invalid";
                case SDL_CONTROLLER_AXIS_LEFTX:
                    return "Left X-Axis";
                case SDL_CONTROLLER_AXIS_LEFTY:
                    return "Left Y-Axis";
                case SDL_CONTROLLER_AXIS_RIGHTX:
                    return "Right X-Axis";
                case SDL_CONTROLLER_AXIS_RIGHTY:
                    return "Right Y-Axis";
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                    return "LT";
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                    return "RT";
                case SDL_CONTROLLER_AXIS_MAX:
                default:
                    return "Unknown";
            }
        case SDL_CONTROLLER_TYPE_PS3:
        case SDL_CONTROLLER_TYPE_PS4:
        case SDL_CONTROLLER_TYPE_PS5:
            switch(axis) {
                case SDL_CONTROLLER_AXIS_INVALID:
                    return "Invalid";
                case SDL_CONTROLLER_AXIS_LEFTX:
                    return "Left X-Axis";
                case SDL_CONTROLLER_AXIS_LEFTY:
                    return "Left Y-Axis";
                case SDL_CONTROLLER_AXIS_RIGHTX:
                    return "Right X-Axis";
                case SDL_CONTROLLER_AXIS_RIGHTY:
                    return "Right Y-Axis";
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                    return "L2";
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                    return "R2";
                case SDL_CONTROLLER_AXIS_MAX:
                default:
                    return "Unknown";
            }
        case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
            switch(axis) {
                case SDL_CONTROLLER_AXIS_INVALID:
                    return "Invalid";
                case SDL_CONTROLLER_AXIS_LEFTX:
                    return "Left X-Axis";
                case SDL_CONTROLLER_AXIS_LEFTY:
                    return "Left Y-Axis";
                case SDL_CONTROLLER_AXIS_RIGHTX:
                    return "Right X-Axis";
                case SDL_CONTROLLER_AXIS_RIGHTY:
                    return "Right Y-Axis";
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                    return "ZL";
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                    return "ZR";
                case SDL_CONTROLLER_AXIS_MAX:
                default:
                    return "Unknown";
            }
    }
}

std::string SDLDevice::GetButtonNameFromBitmask(int mask) {
    // Really think I should have used an Enum here.
    switch(mask) {
        case 0:
            return "None";
        case GAMEPAD_A:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_A);
        case GAMEPAD_B:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_B);
        case GAMEPAD_X:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_X);
        case GAMEPAD_Y:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_Y);
        case GAMEPAD_BACK:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_BACK);
        case GAMEPAD_GUIDE:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_GUIDE);
        case GAMEPAD_START:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_START);
        case GAMEPAD_LEFTSTICK:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_LEFTSTICK);
        case GAMEPAD_RIGHTSTICK:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_RIGHTSTICK);
        case GAMEPAD_LEFTSHOULDER:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        case GAMEPAD_RIGHTSHOULDER:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        case GAMEPAD_DPAD_UP:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_DPAD_UP);
        case GAMEPAD_DPAD_DOWN:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        case GAMEPAD_DPAD_LEFT:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        case GAMEPAD_DPAD_RIGHT:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        case GAMEPAD_MISC1:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_MISC1);
        case GAMEPAD_PADDLE1:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_PADDLE1);
        case GAMEPAD_PADDLE2:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_PADDLE2);
        case GAMEPAD_PADDLE3:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_PADDLE3);
        case GAMEPAD_PADDLE4:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_PADDLE4);
        case GAMEPAD_TOUCHPAD:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_TOUCHPAD);
        case GAMEPAD_TRIGGER_LEFT:
            return GetAxisNameForDevice(SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        case GAMEPAD_TRIGGER_RIGHT:
            return GetAxisNameForDevice(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        default:
            return GetButtonNameForDevice(SDL_CONTROLLER_BUTTON_INVALID);
    }
}

std::string SDLDevice::GetGenericButtonNameFromBitmask(int mask) {
    switch(mask) {
        case 0:
            return "None";
        case GAMEPAD_A:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_A);
        case GAMEPAD_B:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_B);
        case GAMEPAD_X:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_X);
        case GAMEPAD_Y:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_Y);
        case GAMEPAD_BACK:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_BACK);
        case GAMEPAD_GUIDE:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_GUIDE);
        case GAMEPAD_START:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_START);
        case GAMEPAD_LEFTSTICK:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_LEFTSTICK);
        case GAMEPAD_RIGHTSTICK:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_RIGHTSTICK);
        case GAMEPAD_LEFTSHOULDER:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
        case GAMEPAD_RIGHTSHOULDER:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
        case GAMEPAD_DPAD_UP:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_DPAD_UP);
        case GAMEPAD_DPAD_DOWN:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
        case GAMEPAD_DPAD_LEFT:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
        case GAMEPAD_DPAD_RIGHT:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
        case GAMEPAD_MISC1:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_MISC1);
        case GAMEPAD_PADDLE1:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_PADDLE1);
        case GAMEPAD_PADDLE2:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_PADDLE2);
        case GAMEPAD_PADDLE3:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_PADDLE3);
        case GAMEPAD_PADDLE4:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_PADDLE4);
        case GAMEPAD_TOUCHPAD:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_TOUCHPAD);
        case GAMEPAD_TRIGGER_LEFT:
            return GetGenericAxisNames(SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        case GAMEPAD_TRIGGER_RIGHT:
            return GetGenericAxisNames(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        default:
            return GetGenericButtonNames(SDL_CONTROLLER_BUTTON_INVALID);
    }
}

std::string SDLDevice::GetGenericButtonNames(SDL_GameControllerButton button) {
    switch(button) {
        case SDL_CONTROLLER_BUTTON_A:
            return "A";
        case SDL_CONTROLLER_BUTTON_B:
            return "B";
        case SDL_CONTROLLER_BUTTON_BACK:
            return "Back";
        case SDL_CONTROLLER_BUTTON_INVALID:
            return "Invalid";
        case SDL_CONTROLLER_BUTTON_X:
            return "X";
        case SDL_CONTROLLER_BUTTON_Y:
            return "Y";
        case SDL_CONTROLLER_BUTTON_GUIDE:
            return "Home";
        case SDL_CONTROLLER_BUTTON_START:
            return "Start";
        case SDL_CONTROLLER_BUTTON_LEFTSTICK:
            return "LS";
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
            return "RS";
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
            return "LB";
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
            return "RB";
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            return "Up";
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            return "Down";
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            return "Left";
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            return "Right";
        case SDL_CONTROLLER_BUTTON_MISC1:
            return "Misc 1";
        case SDL_CONTROLLER_BUTTON_PADDLE1:
            return "Paddle 1";
        case SDL_CONTROLLER_BUTTON_PADDLE2:
            return "Paddle 2";
        case SDL_CONTROLLER_BUTTON_PADDLE3:
            return "Paddle 3";
        case SDL_CONTROLLER_BUTTON_PADDLE4:
            return "Paddle 4";
        case SDL_CONTROLLER_BUTTON_TOUCHPAD:
            return "Touchpad";
        case SDL_CONTROLLER_BUTTON_MAX:
        default:
            return "Invalid";
    }
}

std::string SDLDevice::GetButtonNameForDevice(SDL_GameControllerButton button) {
    switch(_controllerType) {
        case SDL_CONTROLLER_TYPE_PS4:
        case SDL_CONTROLLER_TYPE_PS5:
        case SDL_CONTROLLER_TYPE_PS3:
            switch(button) {
                case SDL_CONTROLLER_BUTTON_A:
                    return "Cross";
                case SDL_CONTROLLER_BUTTON_B:
                    return "Circle";
                case SDL_CONTROLLER_BUTTON_BACK:
                    if(_controllerType == SDL_CONTROLLER_TYPE_PS5) return "Create";
                    else if(_controllerType == SDL_CONTROLLER_TYPE_PS4) return "Share";
                    else return "Select";
                case SDL_CONTROLLER_BUTTON_INVALID:
                    return "Invalid";
                case SDL_CONTROLLER_BUTTON_X:
                    return "Square";
                case SDL_CONTROLLER_BUTTON_Y:
                    return "Triangle";
                case SDL_CONTROLLER_BUTTON_GUIDE:
                    return "PS";
                case SDL_CONTROLLER_BUTTON_START:
                    if(_controllerType == SDL_CONTROLLER_TYPE_PS5 || _controllerType == SDL_CONTROLLER_TYPE_PS4) return "Options";
                    else return "Select";
                case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                    return "L3";
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                    return "R3";
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    return "L1";
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    return "L2";
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    return "Up";
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    return "Down";
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    return "Left";
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    return "Right";
                case SDL_CONTROLLER_BUTTON_MISC1:
                    if(_controllerType == (SDL_CONTROLLER_TYPE_PS5)) return "Mic";
                    else return "Misc 1";
                case SDL_CONTROLLER_BUTTON_PADDLE1:
                    return "Paddle 1";
                case SDL_CONTROLLER_BUTTON_PADDLE2:
                    return "Paddle 2";
                case SDL_CONTROLLER_BUTTON_PADDLE3:
                    return "Paddle 3";
                case SDL_CONTROLLER_BUTTON_PADDLE4:
                    return "Paddle 4";
                case SDL_CONTROLLER_BUTTON_TOUCHPAD:
                    return "Touchpad";
                case SDL_CONTROLLER_BUTTON_MAX:
                default:
                    return "Invalid";
            }
        case SDL_CONTROLLER_TYPE_UNKNOWN:
            return "Unknown";
        case SDL_CONTROLLER_TYPE_XBOX360:
        case SDL_CONTROLLER_TYPE_XBOXONE:
        case SDL_CONTROLLER_TYPE_VIRTUAL:
        case SDL_CONTROLLER_TYPE_AMAZON_LUNA:
        case SDL_CONTROLLER_TYPE_GOOGLE_STADIA:
            switch(button) {
                case SDL_CONTROLLER_BUTTON_A:
                    return "A";
                case SDL_CONTROLLER_BUTTON_B:
                    return "B";
                case SDL_CONTROLLER_BUTTON_BACK:
                    if(_controllerType == (SDL_CONTROLLER_TYPE_XBOX360)) return "Back";
                    else if(_controllerType == SDL_CONTROLLER_TYPE_XBOXONE) return "View";
                case SDL_CONTROLLER_BUTTON_INVALID:
                    return "Invalid";
                case SDL_CONTROLLER_BUTTON_X:
                    return "X";
                case SDL_CONTROLLER_BUTTON_Y:
                    return "Y";
                case SDL_CONTROLLER_BUTTON_GUIDE:
                    return "Guide";
                case SDL_CONTROLLER_BUTTON_START:
                    if(_controllerType == (SDL_CONTROLLER_TYPE_XBOX360)) return "Start";
                    else if(_controllerType == SDL_CONTROLLER_TYPE_XBOXONE) return "Menu";
                case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                    return "LS";
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                    return "RS";
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    return "LB";
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    return "RB";
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    return "Up";
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    return "Down";
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    return "Left";
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    return "Right";
                case SDL_CONTROLLER_BUTTON_MISC1:
                    if(_controllerType == (SDL_CONTROLLER_TYPE_XBOXONE)) return "Share";
                    else return "Misc 1";
                case SDL_CONTROLLER_BUTTON_PADDLE1:
                    if(_controllerType == (SDL_CONTROLLER_TYPE_XBOXONE)) return "P1";
                    else return "Paddle 1";
                case SDL_CONTROLLER_BUTTON_PADDLE2:
                    if(_controllerType == (SDL_CONTROLLER_TYPE_XBOXONE)) return "P2";
                    else return "Paddle 2";
                case SDL_CONTROLLER_BUTTON_PADDLE3:
                    if(_controllerType == (SDL_CONTROLLER_TYPE_XBOXONE)) return "P3";
                    else return "Paddle 3";
                case SDL_CONTROLLER_BUTTON_PADDLE4:
                    if(_controllerType == (SDL_CONTROLLER_TYPE_XBOXONE)) return "P4";
                    else return "Paddle 4";
                case SDL_CONTROLLER_BUTTON_TOUCHPAD:
                    return "Touchpad";
                case SDL_CONTROLLER_BUTTON_MAX:
                default:
                    return "Invalid";
            }
        case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
            switch(button) {
                case SDL_CONTROLLER_BUTTON_A:
                    return "B";
                case SDL_CONTROLLER_BUTTON_B:
                    return "A";
                case SDL_CONTROLLER_BUTTON_BACK:
                    return "Minus";
                case SDL_CONTROLLER_BUTTON_INVALID:
                    return "Invalid";
                case SDL_CONTROLLER_BUTTON_X:
                    return "Y";
                case SDL_CONTROLLER_BUTTON_Y:
                    return "X";
                case SDL_CONTROLLER_BUTTON_GUIDE:
                    return "Home";
                case SDL_CONTROLLER_BUTTON_START:
                    return "Plus";
                case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                    return "LS";
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                    return "RS";
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                    return "L";
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    return "R";
                case SDL_CONTROLLER_BUTTON_DPAD_UP:
                    return "Up";
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                    return "Down";
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    return "Left";
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    return "Right";
                case SDL_CONTROLLER_BUTTON_MISC1:
                    return "Capture";
                case SDL_CONTROLLER_BUTTON_PADDLE1:
                    return "Paddle 1 (NP)";
                case SDL_CONTROLLER_BUTTON_PADDLE2:
                    return "Paddle 2 (NP)";
                case SDL_CONTROLLER_BUTTON_PADDLE3:
                    return "Paddle 3 (NP)";
                case SDL_CONTROLLER_BUTTON_PADDLE4:
                    return "Paddle 4 (NP)";
                case SDL_CONTROLLER_BUTTON_TOUCHPAD:
                    return "Touchpad (NP)";
                case SDL_CONTROLLER_BUTTON_MAX:
                default:
                    return "None";
            }
    }
}

std::string SDLDevice::GetControllerName() {
    return _controllerName;
}

MotionReport SDLDevice::GetCurrentMotionReport(float deltatime) {
    MotionReport report;
    if(!_deviceHasGyroscope) {
        report.GyroX = 0;
        report.GyroY = 0;
        report.GyroZ = 0;
    }
    else {
        // Standard gyroscope supports 3 axis entry: x, y, z
        float gyroscope[3];
        SDL_GameControllerGetSensorData(_sdlgcptr, SDL_SENSOR_GYRO, gyroscope, 3);
        // Convert from radians to degrees.
        report.GyroX = gyroscope[0] * 180 / PI;
        report.GyroY = gyroscope[1] * 180 / PI;
        report.GyroZ = gyroscope[2] * 180 / PI;
    }

    if(!_deviceHasAccelerometer) {
        report.AccelX = 0;
        report.AccelY = 0;
        report.AccelZ = 0;
    }
    else {
        float accelerometer[3];
        SDL_GameControllerGetSensorData(_sdlgcptr, SDL_SENSOR_GYRO, accelerometer, 3);

        report.AccelX = accelerometer[0];
        report.AccelY = accelerometer[1];
        report.AccelZ = accelerometer[2];
    }
    // It's processing time.
    _gyrocontrol.ProcessMotion(report.GyroX, report.GyroY, report.GyroZ, report.AccelX, report.AccelY, report.AccelZ, deltatime);
    _gyrocontrol.GetCalibratedGyro(report.GyroX, report.GyroY, report.GyroZ);

    return report;
}

unsigned int SDLDevice::GetCurrentButtonState() {
    unsigned int buttons_pressed = 0;

    for(int btn = SDL_CONTROLLER_BUTTON_A; btn < SDL_CONTROLLER_BUTTON_MAX; btn++) {
        auto button = SDL_GameControllerGetButton(_sdlgcptr, static_cast<SDL_GameControllerButton>(btn));
        if(button > 0) buttons_pressed |= (1UL << btn);
    }
    return buttons_pressed;
}

AxisReport SDLDevice::GetCurrentAxisReport() {
    AxisReport report;
    Sint16 val;
    for(int axis = SDL_CONTROLLER_AXIS_LEFTX; axis < SDL_CONTROLLER_AXIS_MAX; axis++) {
        auto axisdata = SDL_GameControllerGetAxis(_sdlgcptr, static_cast<SDL_GameControllerAxis>(axis));
        switch(static_cast<SDL_GameControllerAxis>(axis)) {
            case SDL_CONTROLLER_AXIS_INVALID:
                break;
            case SDL_CONTROLLER_AXIS_LEFTX:
                report.LStick.x = PluginHelpers::ClampFloat(axisdata < 0 ?
                                                            (float)axisdata / 32768 :
                                                            (float)axisdata / 32767,
                                                            -1,
                                                            1);
                break;
            case SDL_CONTROLLER_AXIS_LEFTY:
                report.LStick.y = PluginHelpers::ClampFloat(axisdata < 0 ?
                                                            (float)axisdata / 32768 :
                                                            (float)axisdata / 32767,
                                                            -1,
                                                            1);
                break;
            case SDL_CONTROLLER_AXIS_RIGHTX:
                report.RStick.x = PluginHelpers::ClampFloat(axisdata < 0 ?
                                                            (float)axisdata / 32768 :
                                                            (float)axisdata / 32767,
                                                            -1,
                                                            1);
                break;
            case SDL_CONTROLLER_AXIS_RIGHTY:
                report.RStick.y = PluginHelpers::ClampFloat(axisdata < 0 ?
                                                            (float)axisdata / -std::numeric_limits<Sint16>::lowest() :
                                                            (float)axisdata / std::numeric_limits<Sint16>::max(),
                                                            -1,
                                                            1);
                break;
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                report.LTrigger = PluginHelpers::ClampFloat((float)axisdata / 32767,
                                                            0,
                                                            1);
                break;
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                report.RTrigger = PluginHelpers::ClampFloat((float)axisdata / 32767,
                                                            0,
                                                            1);
                break;
            case SDL_CONTROLLER_AXIS_MAX:
            default:
                break;
        }
    }
    return report;
}

int SDLDevice::GetFirstPressedButton() {
    // Start by forcing a device update to latch controls.
    SDL_GameControllerUpdate();
    AxisReport axis;
    unsigned int buttons;
    axis = GetCurrentAxisReport();
    if(axis.LTrigger > 0.50) {
        return GAMEPAD_TRIGGER_LEFT;
    }
    if(axis.RTrigger > 0.50) {
        return GAMEPAD_TRIGGER_RIGHT;
    }

    auto btns = GetCurrentButtonState();
    int index = 1;

    while((index << 1) <= GAMEPAD_TOUCHPAD ) {
        if(btns & index)
            return index;
        index = index << 1;
    }
    return 0;

}

void SDLDevice::AssignPlayerIndex(int index) {
    _sdlplayerindex = index;
    SDL_GameControllerSetPlayerIndex(_sdlgcptr, index);
}

int SDLDevice::GetPlayerIndex() {
    return _sdlplayerindex;
}

void SDLDevice::StartGyroscopeCalibration() {
    _gyrocontrol.StartContinuousCalibration();
    _calibration_start = std::chrono::steady_clock::now();
    _deviceiscalibrating = true;
}

void SDLDevice::StopGyroscopeCalibration() {
    _gyrocontrol.PauseContinuousCalibration();
    _deviceiscalibrating = false;
}

void SDLDevice::ResetGyroscopeCalibration() {
    _gyrocontrol.ResetContinuousCalibration();
}

bool SDLDevice::GetIfGyroscopeIsCalibrating() const {
    return _deviceiscalibrating;
}

std::chrono::time_point<std::chrono::steady_clock> SDLDevice::GetStartOfLastCalibration() {
    return _calibration_start;
}

void SDLDevice::SetLightbarColor(int color) {
    if(SDL_GameControllerHasLED(_sdlgcptr)) {
        auto r = (color >> 16) & 0xFF;
        auto g = (color >> 8) & 0xFF;
        auto b = color & 0xFF;

        SDL_GameControllerSetLED(_sdlgcptr, r, g, b);
        _colorbar_color = color;
    }
}

int SDLDevice::GetLightbarColor() {
    return _colorbar_color;
}
