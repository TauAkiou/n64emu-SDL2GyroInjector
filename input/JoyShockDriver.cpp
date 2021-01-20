//
// Created by Robin on 1/18/2021.
//

#include <iostream>
#include "JoyShockDriver.h"

JoyShockDriver* JoyShockDriver::instance = nullptr;

JoyShockDriver* JoyShockDriver::getInstance() {
    if(!instance)
        instance = new JoyShockDriver();
    return instance;
}

JoyShockDriver::JoyShockDriver() {
    _gameptr = Game::getInstance();
    _ctrlptr = Controls::getInstance();
    _pluginptr = PluginSettings::getInstance();
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

    while(!_terminatethread) {
        time_previous = time_current;
        time_current = clock();
        delta = ((float)time_current - (float)time_previous) / CLOCKS_PER_SEC;

        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            if(_ctrlptr->Profile[player].SETTINGS[CONFIG] == DISABLED)
                continue;

            PROFILE prf = _ctrlptr->Profile[player];
            DEVICE *dev = &_ctrlptr->Device[player];

            // Handle processing for standard cons (DS4/SPC)
            if(prf.ControllerMode == 0) {
                jsl_buttons_primary = JslGetSimpleState(prf.AssignedDevicePrimary.Handle);
                jsl_imu_primary = JslGetIMUState(prf.AssignedDevicePrimary.Handle);
                // Skip over touch output for now until we can figure out how to use it properly.
                jsl_touch = JslGetTouchState(prf.AssignedDevicePrimary.Handle);

                if(jsl_buttons_primary.stickRX >= 0.10 || jsl_buttons_primary.stickRX <= -0.10) {
                    dev->AIMSTICKX = jsl_buttons_primary.stickRX;
                    dev->AIMSTICKY = jsl_buttons_primary.stickRY;
                }
                else {
                    dev->AIMSTICKX = 0;
                }

                if(jsl_buttons_primary.stickRY >= 0.10 || jsl_buttons_primary.stickRY <= -0.10) {
                    dev->AIMSTICKY = jsl_buttons_primary.stickRY;
                }
                else {
                    dev->AIMSTICKY = 0;
                }

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

                dev->GYROX = -jsl_imu_primary.gyroY;
                dev->GYROY = -jsl_imu_primary.gyroX;

                for (int button = FIRE; button < TOTALBUTTONS; button++) {
                    dev->BUTTONPRIM[button] = (jsl_buttons_primary.buttons & prf.BUTTONPRIM[button].Button) != 0;
                    dev->BUTTONSEC[button] = (jsl_buttons_primary.buttons & prf.BUTTONSEC[button].Button) != 0;
                }

            }
            else if(prf.ControllerMode == 1) {

            }
        }

        if(checkwindowtick > (250 / TICKRATE)) // poll every 500ms
        {
            checkwindowtick = 0;
            if(_emulatorwindow != GetForegroundWindow()) // don't send input if the window is inactive.
            {
                memset(&_ctrlptr->Device, 0, sizeof(DEVICE)); // reset player input
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

}

int JoyShockDriver::GetConnectedDeviceCount() const {
    return _devicecount;
}

void JoyShockDriver::Initialize(const HWND hw) {
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
