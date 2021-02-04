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

#include "JoyShockDriver.h"

JoyShockDriver* JoyShockDriver::instance = nullptr;

JoyShockDriver* JoyShockDriver::getInstance() {
    if(!instance)
        instance = new JoyShockDriver();
    return instance;
}

JoyShockDriver::JoyShockDriver() {
    _gameptr = Game::GetInstance();
    _cstateptr = ControlState::GetInstance();
    _settings = Settings::GetInstance();
}

DWORD JoyShockDriver::injectionloop() {
    // Allocate structs for all handles.
    JOY_SHOCK_STATE jsl_buttons_primary;
    JOY_SHOCK_STATE jsl_buttons_secondary;
    IMU_STATE jsl_imu_primary;
    IMU_STATE jsl_imu_secondary;

    auto time_current = std::chrono::steady_clock::now();
    auto time_previous = time_current;

    int checkwindowtick = 0;

    // Set up controller player lights/colors
    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        auto assignment = _settings->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
        auto profile = _settings->GetProfileForPlayer(static_cast<PLAYERS>(player));
        switch(assignment.ControllerMode) {
            case FULLCONTROLLER:
                JslSetLightColour(assignment.PrimaryDevice.Handle, profile.DS4Color);
                JslSetPlayerNumber(assignment.PrimaryDevice.Handle, player+1);
                break;
            case JOYCONS:
                JslSetPlayerNumber(assignment.PrimaryDevice.Handle, player+1);
                JslSetPlayerNumber(assignment.SecondaryDevice.Handle, player+1);
                break;
            default:
                break;
        }
    }

    auto emutickrate = TICKRATE;
    while(!_terminatethread) {
        time_previous = time_current;
        time_current = std::chrono::steady_clock::now();
        std::chrono::duration<float> dur = time_current - time_previous;

        _cstateptr->DeltaTime = dur.count();

        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            if(!_settings->GetIfPlayerIsConnected(static_cast<PLAYERS>(player)))
                continue;

            PROFILE prf = _settings->GetProfileForPlayer(static_cast<PLAYERS>(player));
            Assignment asgn = _settings->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
            DEVICE *dev = &_cstateptr->Device[player];

            // Handle processing for standard cons (DS4/SPC)
            if(asgn.ControllerMode == FULLCONTROLLER) {
                jsl_buttons_primary = JslGetSimpleState(asgn.PrimaryDevice.Handle);
                jsl_imu_primary = JslGetIMUState(asgn.PrimaryDevice.Handle);

                if(!dev[player].CALIBRATING) {
                    if(jsl_buttons_primary.buttons & prf.BUTTONPRIM[CALIBRATEGYRO] && (time_current - dev[player].CALIBRATIONSTARTTIME).count() > 6) {
                        std::cout << "Player " << player+1 << " controller is calibrating." << std::endl;
                        dev[player].CALIBRATING = true;
                        dev[player].CALIBRATIONSTARTTIME = time_current;
                        JslResetContinuousCalibration(asgn.PrimaryDevice.Handle);
                        JslStartContinuousCalibration(asgn.PrimaryDevice.Handle);
                    }
                }
                else {
                    std::chrono::duration<float> cal_duration = time_current - dev[player].CALIBRATIONSTARTTIME;
                    if(cal_duration.count() > 5.0f) {
                        dev[player].CALIBRATING = false;
                        std::cout << "Player " << player+1 << " calibration complete." << std::endl;
                        JslPauseContinuousCalibration(asgn.PrimaryDevice.Handle);
                    }
                    continue;
                }


                dev->AIMSTICK.y = -jsl_buttons_primary.stickRY;
                dev->AIMSTICK.x = jsl_buttons_primary.stickRX;


                // TODO: Assign variable thresholds rather then using a constant value.
                if (jsl_buttons_primary.lTrigger >= 0.50) // Ensure button bits are set based on threshold.
                    jsl_buttons_primary.buttons |= 1 << JSOFFSET_ZL;
                else
                    jsl_buttons_primary.buttons &= ~(1 << JSOFFSET_ZL);

                if (jsl_buttons_primary.rTrigger >= 0.50)
                    jsl_buttons_primary.buttons |= 1 << JSOFFSET_ZR;
                else
                    jsl_buttons_primary.buttons &= ~(1 << JSOFFSET_ZR);

                // Assign IMU values.

                dev->GYRO.x = (jsl_imu_primary.gyroY > 0.2 || jsl_imu_primary.gyroY < -0.2) ? -jsl_imu_primary.gyroY : 0;
                dev->GYRO.y = (jsl_imu_primary.gyroX > 0.2 || jsl_imu_primary.gyroX < -0.2) ? -jsl_imu_primary.gyroX : 0;

                for (int button = FIRE; button < TOTALBUTTONS; button++) {
                    dev->BUTTONPRIM[button] = (jsl_buttons_primary.buttons & prf.BUTTONPRIM[button]) != 0;
                    dev->BUTTONSEC[button] = (jsl_buttons_primary.buttons & prf.BUTTONSEC[button]) != 0;
                }

                // Process stick logic last to get around the button overwrite for movement.
                dev->BUTTONPRIM[FORWARDS] = jsl_buttons_primary.stickLY > 0.25;
                dev->BUTTONPRIM[BACKWARDS] = jsl_buttons_primary.stickLY < -0.25;
                dev->BUTTONPRIM[STRAFELEFT] = jsl_buttons_primary.stickLX < -0.25;
                dev->BUTTONPRIM[STRAFERIGHT] = jsl_buttons_primary.stickLX > 0.25;
            }
            else if(asgn.ControllerMode == JOYCONS) {
                // Primary is our left joycon, secondary is our right joycon.
                jsl_buttons_primary = JslGetSimpleState(asgn.PrimaryDevice.Handle);
                jsl_buttons_secondary = JslGetSimpleState(asgn.SecondaryDevice.Handle);
                jsl_imu_primary = JslGetIMUState(asgn.PrimaryDevice.Handle);
                jsl_imu_secondary = JslGetIMUState(asgn.SecondaryDevice.Handle);
                auto combined_buttons = jsl_buttons_primary.buttons | jsl_buttons_secondary.buttons;

                if(!dev->CALIBRATING) {
                    if(combined_buttons & prf.CalibrationButton) {
                        std::cout << "Player " << player+1 << " joycons are calibrating." << std::endl;
                        dev->CALIBRATIONSTARTTIME = time_current;
                        dev->CALIBRATING = true;
                        JslResetContinuousCalibration(asgn.PrimaryDevice.Handle);
                        JslResetContinuousCalibration(asgn.SecondaryDevice.Handle);
                        JslStartContinuousCalibration(asgn.PrimaryDevice.Handle);
                        JslStartContinuousCalibration(asgn.SecondaryDevice.Handle);
                    }
                }
                else {
                    auto cal_duration = time_current - dev->CALIBRATIONSTARTTIME;
                    if(cal_duration.count() < 5.0f) {
                        std::cout << "Player " << player+1 << " calibration complete." << std::endl;

                        dev->CALIBRATING = false;
                        JslPauseContinuousCalibration(asgn.PrimaryDevice.Handle);
                        JslPauseContinuousCalibration(asgn.SecondaryDevice.Handle);
                    }
                }

                // Aimstick is typically on the secondary device.
                dev->AIMSTICK.y = -jsl_buttons_secondary.stickRY;
                dev->AIMSTICK.x = jsl_buttons_secondary.stickRX;

                dev->GYRO.x = (jsl_imu_secondary.gyroY > 0.2 || jsl_imu_secondary.gyroY < -0.2) ? -jsl_imu_secondary.gyroY : 0;
                dev->GYRO.y = (jsl_imu_secondary.gyroX > 0.2 || jsl_imu_secondary.gyroX < -0.2) ? -jsl_imu_secondary.gyroX : 0;

                for (int button = FIRE; button < TOTALBUTTONS; button++) {
                    dev->BUTTONPRIM[button] = (combined_buttons & prf.BUTTONPRIM[button]) != 0;
                    dev->BUTTONSEC[button] = (combined_buttons & prf.BUTTONSEC[button]) != 0;
                }

                dev->BUTTONPRIM[FORWARDS] = jsl_buttons_primary.stickLY > 0.25;
                dev->BUTTONPRIM[BACKWARDS] = jsl_buttons_primary.stickLY < -0.25;
                dev->BUTTONPRIM[STRAFELEFT] = jsl_buttons_primary.stickLX < -0.25;
                dev->BUTTONPRIM[STRAFERIGHT] = jsl_buttons_primary.stickLX > 0.25;
            }
        }

        if(checkwindowtick > (250 / emutickrate)) // poll every 500ms
        {
            // Hijack this to ensure we can actually read the gyro output i guess

            //std::cout << "GyroX: " << _ctrlptr->Device[PLAYER1].GYROX << " GyroY: " << _ctrlptr->Device[PLAYER1].GYROY << std::endl;
            checkwindowtick = 0;
            if(_emulatorwindow != GetForegroundWindow() || _looppaused) // don't send input if the window is inactive.
            {
                memset(&_cstateptr->Device, 0, sizeof(DEVICE)); // reset player input
                _gameptr->Inject(); // ship empty input to game
                _windowactive = 0;
            }
            else {
                _windowactive = 1;
            }
        }
        else
            checkwindowtick++;


        if(_windowactive && _gameptr->Status() && !_looppaused) { // if emulator is focused, game is valid and config dialog isn't open

            _gameptr->Inject(); // send input to game driver
        }
        Sleep(emutickrate); // 2ms (500 Hz) for overclocked, 4ms (250 Hz) for stock speed

    }
    _gameptr->Quit();
    return 0;
}

int JoyShockDriver::GetConnectedDeviceCount() const {
    return _devicecount;
}

int JoyShockDriver::Initialize(const HWND hw) {
    if(!_initialized) {
        _devicecount = JslConnectDevices();

        int* discoveredhandles = new int[_devicecount];
        JslGetConnectedDeviceHandles(discoveredhandles, _devicecount);

        JSDevice deviceentry;

        for(int _handleid = 0; _handleid < _devicecount; _handleid++) {
            deviceentry.Handle = _handleid;
            deviceentry.Type = (JSD_ControllerType)JslGetControllerType(_handleid);

            _devices->push_back(deviceentry);
        }
        delete[] discoveredhandles;
        _initialized = true;
    }
    return _devicecount;

}

void JoyShockDriver::AssignEmulatorWindow(const HWND hw) {
    if(_emulatorwindow == nullptr) {
        _emulatorwindow = hw;
    }
}

DWORD JoyShockDriver::startinjectionloop(void* param) {
    JoyShockDriver* ThisClass = (JoyShockDriver*) param;
    return ThisClass->injectionloop();
}


void JoyShockDriver::StartInjectionThread() {
    if(_terminatethread) {
        _inputthread = CreateThread(nullptr, 0, startinjectionloop, (void*)this, 0, nullptr);
        _terminatethread = false;
    }
}

void JoyShockDriver::EndInjectionThread() {
    if(!_terminatethread) {
        CloseHandle(_inputthread);
        _terminatethread = true;
        _inputthread = nullptr;
    }
}

std::vector<JSDevice> JoyShockDriver::GetConnectedFullControllers() {
    std::vector<JSDevice> val;
    for(JSDevice jsd : *_devices) {
        if(jsd.Type == SwitchPro || jsd.Type == Dualshock_4 || jsd.Type == Dualsense)
            val.push_back(jsd);
    }
    return val;
}

std::vector<JSDevice> JoyShockDriver::GetConnectedDS4() {
    std::vector<JSDevice> val;
    for(JSDevice jsd : *_devices) {
        if(jsd.Type == Dualshock_4)
            val.push_back(jsd);
    }
    return val;
}

std::vector<JSDevice> JoyShockDriver::GetConnectedLeftJoycons() {
    std::vector<JSDevice> val;
    for(JSDevice jsd : *_devices) {
        if(jsd.Type == JoyconLeft)
            val.push_back(jsd);
    }
    return val;
}

std::vector<JSDevice> JoyShockDriver::GetConnectedRightJoycons() {
    std::vector<JSDevice> val;
    for(JSDevice jsd : *_devices) {
        if(jsd.Type == JoyconRight)
            val.push_back(jsd);
    }
    return val;
}


void JoyShockDriver::CalibrateAllGyroscopes() {
    std::cout << "Starting calibration for all assigned controllers..." << std::endl;
    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        auto asgn = _settings->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
        JslResetContinuousCalibration(asgn.PrimaryDevice.Handle);
        JslStartContinuousCalibration(asgn.PrimaryDevice.Handle);
        if(asgn.ControllerMode > 1 && asgn.SecondaryDevice.Handle != -1) {
            JslResetContinuousCalibration(asgn.SecondaryDevice.Handle);
            JslStartContinuousCalibration(asgn.SecondaryDevice.Handle);
        }
    }
    auto clock_start = clock();
    auto clock_now = clock_start;

    while((clock_now - clock_start) / CLOCKS_PER_SEC < 2)  {
        clock_now = clock();
    }

    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        auto asgn = _settings->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
        JslPauseContinuousCalibration(asgn.PrimaryDevice.Handle);
        if(asgn.ControllerMode > 1 && asgn.SecondaryDevice.Handle != -1) {
            JslPauseContinuousCalibration(asgn.SecondaryDevice.Handle);
        }
    }
    std::cout << "Calibration completed." << std::endl;
}

std::string JoyShockDriver::GetButtonLabelForController(JSDevice device, int buttonmask) {
    if(device.Type == Dualshock_4 || device.Type == Dualsense) {
        switch (buttonmask) {
            case JSMASK_UP:
                return "D-Pad Up";
            case JSMASK_DOWN:
                return "D-Pad Down";
            case JSMASK_LEFT:
                return "D-Pad Left";
            case JSMASK_RIGHT:
                return "D-Pad Right";
            case JSMASK_OPTIONS:
                return "Options";
            case JSMASK_SHARE:
                if(device.Type == Dualsense) return "Create";
                return "Share";
            case JSMASK_LCLICK:
                return "L3";
            case JSMASK_RCLICK:
                return "R3";
            case JSMASK_L:
                return "L1";
            case JSMASK_R:
                return "R1";
            case JSMASK_ZL:
                return "L2";
            case JSMASK_ZR:
                return "R2";
            case JSMASK_S:
                return "Cross";
            case JSMASK_E:
                return "Circle";
            case JSMASK_W:
                return "Square";
            case JSMASK_N:
                return "Triangle";
            case JSMASK_PS:
                return "PS";
            case JSMASK_TOUCHPAD_CLICK:
                return "Touchpad";
            case JSMASK_SL:
                return "Mic";
            default:
                return "None";
        }
    }
    else if(device.Type == SwitchPro || device.Type == JoyconLeft || device.Type == JoyconRight || device.Type == None) {
        switch(buttonmask) {
             case JSMASK_UP:
                return "D-Pad Up";
                case JSMASK_DOWN:
                    return "D-Pad Down";
                case JSMASK_LEFT:
                    return "D-Pad Left";
                case JSMASK_RIGHT:
                    return "D-Pad Right";
                case JSMASK_PLUS:
                    return "+";
                case JSMASK_MINUS:
                    return "-";
                case JSMASK_LCLICK:
                    return "LS-Click";
                case JSMASK_RCLICK:
                    return "RS-Click";
                case JSMASK_L:
                    return "L";
                case JSMASK_R:
                    return "R";
                case JSMASK_ZL:
                    return "ZL";
                case JSMASK_ZR:
                    return "ZR";
                case JSMASK_S:
                    return "B";
                case JSMASK_E:
                    return "A";
                case JSMASK_W:
                    return "Y";
                case JSMASK_N:
                    return "X";
                case JSMASK_HOME:
                    return "Home";
                case JSMASK_CAPTURE:
                    return "Capture";
            case JSMASK_SL:
                return "SL";
            case JSMASK_SR:
                return "SR";
                default:
                    return "None";
            }
    }
    return "None";
}

std::string JoyShockDriver::GetNameOfDevice(JSDevice &device) {
    switch(device.Type) {
        case SwitchPro:
            return "Switch Pro Controller";
        case Dualshock_4:
            return "Dualshock 4";
        case Dualsense:
            return "DualSense";
        case JoyconLeft:
            return "Left Joycon";
        case JoyconRight:
            return "Right Joycon";
        default:
            return "Unknown";
    }
}

void JoyShockDriver::ReconnectControllers() {
    _devices->clear();
    _devicecount = JslConnectDevices();

    int* discoveredhandles = new int[_devicecount];
    JslGetConnectedDeviceHandles(discoveredhandles, _devicecount);

    JSDevice deviceentry;

    for(int _handleid; _handleid < _devicecount; _handleid++) {
        deviceentry.Handle = _handleid;
        deviceentry.Type = (JSD_ControllerType)JslGetControllerType(_handleid);

        _devices->push_back(deviceentry);
    }
    delete[] discoveredhandles;
}

int JoyShockDriver::GetFirstButtonFromDevice(JSDevice jsd) {
    auto btns = JslGetButtons(jsd.Handle);
    int index = 1;

    while((index << 1) <= JSMASK_SR ) {
        if(btns & index)
            return index;
        index = index << 1;
    }
    return 0;
}

void JoyShockDriver::SetDS4Color(JSDevice dev, int color) {
    if(dev.Handle > -1)
        JslSetLightColour(dev.Handle, color);
}

void JoyShockDriver::SetSPCJCNumber(JSDevice dev, const int number) {
    if(dev.Handle > -1)
        JslSetPlayerNumber(dev.Handle, number);
}

void JoyShockDriver::UnpauseInjection() {
    _looppaused = false;
}

void JoyShockDriver::PauseInjection() {
    _looppaused = true;
}

bool JoyShockDriver::IsThreadRunning() {
    return _terminatethread;
}
