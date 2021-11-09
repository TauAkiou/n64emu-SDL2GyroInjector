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

#include "SdlDriver.h"

SdlDriver* SdlDriver::instance = nullptr;

SdlDriver* SdlDriver::getInstance() {
    if(!instance)
        instance = new SdlDriver();
    return instance;
}

SdlDriver::SdlDriver() {
    _gameptr = Game::GetInstance();
    _cstateptr = ControlState::GetInstance();
    _settings = Settings::GetInstance();
}

DWORD SdlDriver::injectionloop() {
    // Allocate structs for all handles.
    unsigned int sdl_buttons;
    //JOY_SHOCK_STATE jsl_buttons_secondary;
    MotionReport sdl_motionreport;
    AxisReport sdl_axisreport;
    //IMU_STATE jsl_imu_secondary;

    auto time_current = std::chrono::steady_clock::now();
    auto time_previous = time_current;


    int checkwindowtick = 0;

    // Set up controller player lights/colors
    for(int player = PLAYER1; player < ALLPLAYERS; player++) {

    }

    auto emutickrate = TICKRATE;
    while(!_terminatethread) {
        time_previous = time_current;
        time_current = std::chrono::steady_clock::now();
        std::chrono::duration<float> dur = time_current - time_previous;
        _cstateptr->DeltaTime = dur.count();
        // Update the state of all active controllers.
        SDL_GameControllerUpdate();

        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            if(!_settings->GetIfPlayerIsConnected(static_cast<PLAYERS>(player)))
                continue;

            auto assignment = _settings->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
            auto profile = _settings->GetProfileForPlayer(static_cast<PLAYERS>(player));
            auto dev = &_cstateptr->Device[player];

            sdl_buttons = assignment.PrimaryDevice->GetCurrentButtonState();
            sdl_motionreport = assignment.PrimaryDevice->GetCurrentMotionReport(dur.count());
            sdl_axisreport = assignment.PrimaryDevice->GetCurrentAxisReport();

            // All we do with calibration is a simple 5 second calibration routine. Additionally, give a 2 second cooldown between calibrations.
            if(!assignment.PrimaryDevice->GetIfGyroscopeIsCalibrating()) {
                if((sdl_buttons & profile.BUTTONPRIM[CALIBRATEGYRO]) != 0 && (time_current - assignment.PrimaryDevice->GetStartOfLastCalibration()).count() > 6) {
                    std::cout << "Player " << player+1 << " controller is calibrating." << std::endl;
                    assignment.PrimaryDevice->ResetGyroscopeCalibration();
                    assignment.PrimaryDevice->StartGyroscopeCalibration();
                    continue;
                }
            }
            else {
                std::chrono::duration<float> cal_duration = time_current - assignment.PrimaryDevice->GetStartOfLastCalibration();
                auto caldtest = cal_duration.count();
                std::cout << "Time passed: " << caldtest << std::endl;
                if(cal_duration.count() > 5.0f) {
                    assignment.PrimaryDevice->StopGyroscopeCalibration();
                    std::cout << "Player " << player+1 << " calibration complete." << std::endl;
                }
            }


            dev->AIMSTICK.x = sdl_axisreport.RStick.x;
            dev->AIMSTICK.y = sdl_axisreport.RStick.y;

            if(sdl_axisreport.LTrigger >= 0.50)
                sdl_buttons |= 1 << GAMEPAD_OFFSET_TRIGGER_LEFT;

            if(sdl_axisreport.RTrigger >= 0.50)
                sdl_buttons |= 1 << GAMEPAD_OFFSET_TRIGGER_RIGHT;

            dev->GYRO.x = -sdl_motionreport.GyroY;
            dev->GYRO.y = -sdl_motionreport.GyroX;

            for(int button = FIRE; button < TOTALBUTTONS; button++) {
                dev->BUTTONPRIM[button] = (sdl_buttons & profile.BUTTONPRIM[button]) != 0;
                dev->BUTTONSEC[button] = (sdl_buttons & profile.BUTTONSEC[button]) != 0;
            }

            dev->BUTTONPRIM[FORWARDS] = sdl_axisreport.LStick.y < -0.25;
            dev->BUTTONPRIM[BACKWARDS] = sdl_axisreport.LStick.y > 0.25;
            dev->BUTTONPRIM[STRAFELEFT] = sdl_axisreport.LStick.x < -0.25;
            dev->BUTTONPRIM[STRAFERIGHT] = sdl_axisreport.LStick.x > 0.25;

        }

        if(checkwindowtick > (250 / emutickrate)) // poll every 500ms
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


        if(_windowactive && _gameptr->Status() && !_looppaused) { // if emulator is focused, game is valid and config dialog isn't open

            _gameptr->Inject(); // send input to game driver
        }
        Sleep(emutickrate); // 2ms (500 Hz) for overclocked, 4ms (250 Hz) for stock speed

    }
    _gameptr->Quit();
    return 0;
}

int SdlDriver::GetConnectedDeviceCount() const {
    return _devicecount;
}

int SdlDriver::Initialize(const HWND hw) {
    if(!_initialized) {
        // Make sure that these two hints are set to force enable enhanced mode over bluetooth.
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_JOY_CONS, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_SWITCH_HOME_LED, "0");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
        if(SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
            return -1; // SDL failed to initialize.

            auto jstotal = SDL_NumJoysticks();
            for(int index = 0; index < jstotal; ++index) {
                if(SDL_IsGameController(index)) {
                    _controllers.push_back(std::make_shared<SDLDevice>(index));
                }
                std::cout << std::endl;
            }
    }
    return _controllers.size();
}

void SdlDriver::AssignEmulatorWindow(const HWND hw) {
    if(_emulatorwindow == nullptr) {
        _emulatorwindow = hw;
    }
}

DWORD SdlDriver::startinjectionloop(void* param) {
    auto ThisClass = (SdlDriver*) param;
    return ThisClass->injectionloop();
}


void SdlDriver::StartInjectionThread() {
    if(_terminatethread) {
        _inputthread = CreateThread(nullptr, 0, startinjectionloop, (void*)this, 0, nullptr);
        _terminatethread = false;
    }
}

void SdlDriver::EndInjectionThread() {
    if(!_terminatethread) {
        CloseHandle(_inputthread);
        _terminatethread = true;
        _inputthread = nullptr;
    }
}

std::vector<std::shared_ptr<SDLDevice>> SdlDriver::GetConnectedControllers() {
    return std::vector<std::shared_ptr<SDLDevice>>(_controllers);
}

std::vector<SDLDevice> SdlDriver::GetConnectedDS4() {
    return std::vector<SDLDevice>();
}

// Return empty lists for the Joycons since SDL2 does not yet support Joycon logic.
std::vector<SDLDevice> SdlDriver::GetConnectedLeftJoycons() {
    return std::vector<SDLDevice>();
}

std::vector<SDLDevice> SdlDriver::GetConnectedRightJoycons() {
    return std::vector<SDLDevice>();
}


void SdlDriver::CalibrateAllGyroscopes() {
}

std::string SdlDriver::GetButtonLabelForController(SDLDevice device, int buttonmask) {
    return "";
}

std::string SdlDriver::GetNameOfDevice(SDLDevice &device) {
    return "";
}

void SdlDriver::ReconnectControllers() {
}

int SdlDriver::GetFirstButtonFromDevice(SDLDevice &sdldev) {
    return -1;
}

void SdlDriver::SetDS4Color(SDLDevice dev, int color) {
}

void SdlDriver::SetSPCJCNumber(SDLDevice dev, const int number) {
}

void SdlDriver::UnpauseInjection() {
    _looppaused = false;
}

void SdlDriver::PauseInjection() {
    _looppaused = true;
}

bool SdlDriver::IsThreadRunning() {
    return _terminatethread;
}

std::shared_ptr<SDLDevice> SdlDriver::GetFirstController() {
    return _controllers.front();
}
