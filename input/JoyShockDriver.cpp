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

    // AFAIK we have two seperate IMUs in the joycons? I assume that's actually only true for the right joycon.
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
        auto prf = _settings->Profile[player];
        switch(prf.ControllerMode) {
            default:
            case 0:
                JslSetLightColour(prf.AssignedDevicePrimary.Handle, prf.DS4Color);
                JslSetPlayerNumber(prf.AssignedDevicePrimary.Handle, player+1);
                break;
            case 1:
                JslSetPlayerNumber(prf.AssignedDevicePrimary.Handle, player+1);
                JslSetPlayerNumber(prf.AssignedDeviceSecondary.Handle, player+1);
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
            if(_settings->Profile[player].QuickConfigSetting == DISABLED)
                continue;

            PROFILE prf = _settings->Profile[player];
            DEVICE *dev = &_cstateptr->Device[player];



            // Handle processing for standard cons (DS4/SPC)
            if(prf.ControllerMode == 0) {
                jsl_buttons_primary = JslGetSimpleState(prf.AssignedDevicePrimary.Handle);
                jsl_imu_primary = JslGetIMUState(prf.AssignedDevicePrimary.Handle);
                // Skip over touch output for now until we can figure out how to use it properly.
                jsl_touch = JslGetTouchState(prf.AssignedDevicePrimary.Handle);

                if(!dev->GYROCALIBRATION) {
                    if(jsl_buttons_primary.buttons & prf.CalibrationButton) {
                        std::cout << "Player " << player+1 << " controller is calibrating." << std::endl;
                        dev->GYROCALIBRATION = true;
                        JslResetContinuousCalibration(prf.AssignedDevicePrimary.Handle);
                        JslStartContinuousCalibration(prf.AssignedDevicePrimary.Handle);
                    }
                }
                else {
                    if(!(jsl_buttons_primary.buttons & prf.CalibrationButton)) {
                        std::cout << "Player " << player+1 << " calbration complete." << std::endl;
                        dev->GYROCALIBRATION = false;
                        JslPauseContinuousCalibration(prf.AssignedDevicePrimary.Handle);
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
                jsl_buttons_primary = JslGetSimpleState(prf.AssignedDevicePrimary.Handle);
                jsl_buttons_secondary = JslGetSimpleState(prf.AssignedDeviceSecondary.Handle);
                jsl_imu_primary = JslGetIMUState(prf.AssignedDevicePrimary.Handle);
                jsl_imu_secondary = JslGetIMUState(prf.AssignedDeviceSecondary.Handle);
                auto combined_buttons = jsl_buttons_primary.buttons | jsl_buttons_secondary.buttons;

                if(!dev->GYROCALIBRATION) {
                    if(combined_buttons & prf.CalibrationButton) {
                        std::cout << "Player " << player+1 << " joycons are calibrating." << std::endl;

                        dev->GYROCALIBRATION = true;
                        JslResetContinuousCalibration(prf.AssignedDevicePrimary.Handle);
                        JslResetContinuousCalibration(prf.AssignedDeviceSecondary.Handle);
                        JslStartContinuousCalibration(prf.AssignedDevicePrimary.Handle);
                        JslStartContinuousCalibration(prf.AssignedDeviceSecondary.Handle);
                    }
                }
                else {
                    if(!(combined_buttons & prf.CalibrationButton)) {
                        std::cout << "Player " << player+1 << " calibration complete." << std::endl;

                        dev->GYROCALIBRATION = false;
                        JslPauseContinuousCalibration(prf.AssignedDevicePrimary.Handle);
                        JslPauseContinuousCalibration(prf.AssignedDeviceSecondary.Handle);
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
            deviceentry.ConType = (JSD_ControllerType)JslGetControllerType(_handleid);
            deviceentry.DS4LightColor = 0x000000;
            deviceentry.NSPlayerLight = 0;

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
        if(jsd.ConType == SwitchPro || jsd.ConType == Dualshock_4)
            val.push_back(jsd);
    }
    return val;
}

std::vector<JSDevice> JoyShockDriver::GetConnectedLeftJoycons() {
    std::vector<JSDevice> val;
    for(JSDevice jsd : *_devices) {
        if(jsd.ConType == JoyconLeft)
            val.push_back(jsd);
    }
    return val;
}

std::vector<JSDevice> JoyShockDriver::GetConnectedRightJoycons() {
    std::vector<JSDevice> val;
    for(JSDevice jsd : *_devices) {
        if(jsd.ConType == JoyconRight)
            val.push_back(jsd);
    }
    return val;
}


void JoyShockDriver::CalibrateAllGyroscopes() {
    std::cout << "Starting calibration for all assigned controllers..." << std::endl;
    for(PROFILE prf : _settings->Profile) {
        JslResetContinuousCalibration(prf.AssignedDevicePrimary.Handle);
        JslStartContinuousCalibration(prf.AssignedDevicePrimary.Handle);
        if(prf.ControllerMode > 1 && prf.AssignedDeviceSecondary.Handle != -1) {
            JslResetContinuousCalibration(prf.AssignedDeviceSecondary.Handle);
            JslStartContinuousCalibration(prf.AssignedDeviceSecondary.Handle);
        }
    }
    auto clock_start = clock();
    auto clock_now = clock_start;

    while((clock_now - clock_start) / CLOCKS_PER_SEC < 2)  {
        clock_now = clock();
    }

    for(const PROFILE &prf : _settings->Profile) {
        JslPauseContinuousCalibration(prf.AssignedDevicePrimary.Handle);
        if(prf.ControllerMode > 1 && prf.AssignedDeviceSecondary.Handle != -1) {
            JslPauseContinuousCalibration(prf.AssignedDeviceSecondary.Handle);
        }
    }
    std::cout << "Calibration completed." << std::endl;
}
