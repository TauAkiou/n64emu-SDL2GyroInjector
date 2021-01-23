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

    TOUCH_STATE jsl_touch;

    // Allocate variables for system clock.
    clock_t time_current = clock();
    clock_t time_previous;
    float delta;

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

    // Start by calibrating all gyroscopes.
    CalibrateAllGyroscopes();

    while(!_terminatethread) {
        time_previous = time_current;
        time_current = clock();
        _cstateptr->DeltaTime = ((float)time_current - (float)time_previous) / CLOCKS_PER_SEC;

        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            if(_settings->GetIfPlayerIsConnected(static_cast<PLAYERS>(player)))
                continue;

            PROFILE prf = _settings->GetProfileForPlayer(static_cast<PLAYERS>(player));
            Assignment asgn = _settings->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
            DEVICE *dev = &_cstateptr->Device[player];



            // Handle processing for standard cons (DS4/SPC)
            if(prf.ControllerMode == 0) {
                jsl_buttons_primary = JslGetSimpleState(asgn.PrimaryDevice.Handle);
                jsl_imu_primary = JslGetIMUState(asgn.PrimaryDevice.Handle);
                // Skip over touch output for now until we can figure out how to use it properly.
                jsl_touch = JslGetTouchState(asgn.PrimaryDevice.Handle);

                if(!dev->GYROCALIBRATION) {
                    if(jsl_buttons_primary.buttons & prf.CalibrationButton) {
                        std::cout << "Player " << player+1 << " controller is calibrating." << std::endl;
                        dev->GYROCALIBRATION = true;
                        JslResetContinuousCalibration(asgn.PrimaryDevice.Handle);
                        JslStartContinuousCalibration(asgn.PrimaryDevice.Handle);
                    }
                }
                else {
                    if(!(jsl_buttons_primary.buttons & prf.CalibrationButton)) {
                        std::cout << "Player " << player+1 << " calbration complete." << std::endl;
                        dev->GYROCALIBRATION = false;
                        JslPauseContinuousCalibration(asgn.PrimaryDevice.Handle);
                    }
                }


                dev->AIMSTICK.y = -jsl_buttons_primary.stickRY;
                dev->AIMSTICK.x = jsl_buttons_primary.stickRX;

                dev->BUTTONPRIM[FORWARDS] = jsl_buttons_primary.stickLY > 0.25;
                dev->BUTTONPRIM[BACKWARDS] = jsl_buttons_primary.stickLY < -0.25;
                dev->BUTTONPRIM[STRAFELEFT] = jsl_buttons_primary.stickLX < -0.25;
                dev->BUTTONPRIM[STRAFERIGHT] = jsl_buttons_primary.stickLX > 0.25;

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

            }
            else if(prf.ControllerMode == 1) {
                // Primary is our left joycon, secondary is our right joycon.
                jsl_buttons_primary = JslGetSimpleState(asgn.PrimaryDevice.Handle);
                jsl_buttons_secondary = JslGetSimpleState(asgn.SecondaryDevice.Handle);
                jsl_imu_primary = JslGetIMUState(asgn.PrimaryDevice.Handle);
                jsl_imu_secondary = JslGetIMUState(asgn.SecondaryDevice.Handle);
                auto combined_buttons = jsl_buttons_primary.buttons | jsl_buttons_secondary.buttons;

                if(!dev->GYROCALIBRATION) {
                    if(combined_buttons & prf.CalibrationButton) {
                        std::cout << "Player " << player+1 << " joycons are calibrating." << std::endl;

                        dev->GYROCALIBRATION = true;
                        JslResetContinuousCalibration(asgn.PrimaryDevice.Handle);
                        JslResetContinuousCalibration(asgn.SecondaryDevice.Handle);
                        JslStartContinuousCalibration(asgn.PrimaryDevice.Handle);
                        JslStartContinuousCalibration(asgn.SecondaryDevice.Handle);
                    }
                }
                else {
                    if(!(combined_buttons & prf.CalibrationButton)) {
                        std::cout << "Player " << player+1 << " calibration complete." << std::endl;

                        dev->GYROCALIBRATION = false;
                        JslPauseContinuousCalibration(asgn.PrimaryDevice.Handle);
                        JslPauseContinuousCalibration(asgn.SecondaryDevice.Handle);
                    }
                }

                dev->BUTTONPRIM[FORWARDS] = jsl_buttons_primary.stickLY > 0.25;
                dev->BUTTONPRIM[BACKWARDS] = jsl_buttons_primary.stickLY < -0.25;
                dev->BUTTONPRIM[STRAFELEFT] = jsl_buttons_primary.stickLX < -0.25;
                dev->BUTTONPRIM[STRAFERIGHT] = jsl_buttons_primary.stickLX > 0.25;

                // Aimstick is typically on the secondary device.
                dev->AIMSTICK.y = -jsl_buttons_secondary.stickRY;
                dev->AIMSTICK.x = jsl_buttons_secondary.stickRX;

                dev->GYRO.x = (jsl_imu_secondary.gyroY > 0.2 || jsl_imu_secondary.gyroY < -0.2) ? -jsl_imu_secondary.gyroY : 0;
                dev->GYRO.y = (jsl_imu_secondary.gyroX > 0.2 || jsl_imu_secondary.gyroX < -0.2) ? -jsl_imu_secondary.gyroX : 0;

                for (int button = FIRE; button < TOTALBUTTONS; button++) {
                    dev->BUTTONPRIM[button] = (combined_buttons & prf.BUTTONPRIM[button]) != 0;
                    dev->BUTTONSEC[button] = (combined_buttons & prf.BUTTONSEC[button]) != 0;
                }
            }
        }

        if(checkwindowtick > (250 / TICKRATE)) // poll every 500ms
        {
            // Hijack this to ensure we can actually read the gyro output i guess

            //std::cout << "GyroX: " << _ctrlptr->Device[PLAYER1].GYROX << " GyroY: " << _ctrlptr->Device[PLAYER1].GYROY << std::endl;
            checkwindowtick = 0;
            if(_emulatorwindow != GetForegroundWindow()) // don't send input if the window is inactive.
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


        if(_windowactive && _gameptr->Status()) { // if emulator is focused, game is valid and config dialog isn't open

            _gameptr->Inject(); // send input to game driver
        }
        Sleep(TICKRATE); // 2ms (500 Hz) for overclocked, 4ms (250 Hz) for stock speed

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

        int discoveredhandles[_devicecount];
        JslGetConnectedDeviceHandles(discoveredhandles, _devicecount);

        JSDevice deviceentry;

        for(int _handleid : discoveredhandles) {
            deviceentry.Handle = _handleid;
            deviceentry.Type = (JSD_ControllerType)JslGetControllerType(_handleid);

            _devices->push_back(deviceentry);
        }
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
        _inputthread = CreateThread(nullptr, 0, startinjectionloop, (void*)this, 0, NULL);
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
        if(jsd.Type == SwitchPro || jsd.Type == Dualshock_4)
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
