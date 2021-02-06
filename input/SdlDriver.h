/*
 *==========================================================================
 * Joyshock Injector - 1964 Plugin
 *==========================================================================
 * Largely based off of the original Keyboard/Mouse Injector Plugin
 * Copyright (C) 2016-2021 Carnivorous
 *
 * Joyshock Injector
 * Copyright (C) 2021 TauAkiou
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

/*
 * SdlDriver.h - Class for interfacing with SDL2 and operating core injection loop.
 */


#ifndef INC_1964_INPUT_JOYSHOCKCPP_SDLDRIVER_H
#define INC_1964_INPUT_JOYSHOCKCPP_SDLDRIVER_H

#include <vector>
#include <map>
#include <windows.h>
#include <ctime>
#include <iostream>
#include <memory>
#include <SDL.h>
#include <SDL_joystick.h>
#include <SDL_hints.h>
#include <memory>
#include "../common/common.h"
#include "InputClasses.h"
#include "../settings/Settings.h"
#include "../game/Game.h"

class SdlDriver {
    protected:
        static SdlDriver* instance;
        HANDLE _inputthread = nullptr;

        // SDL GameController Instance
        SDL_Joystick* _sdlController = nullptr;

        Settings* _settings = nullptr;
        ControlState* _cstateptr = nullptr;
        Game* _gameptr = nullptr;
        bool _terminatethread = true;
        int _devicecount = 0;
        int _windowactive = 1;
        bool _initialized = false;
        bool _looppaused = false;

        std::vector<std::shared_ptr<SDLDevice>> _controllers;
        HWND _emulatorwindow = nullptr;

        static DWORD WINAPI startinjectionloop(void* param);
        DWORD injectionloop();


    public:
        static SdlDriver* getInstance();
        SdlDriver();
        int Initialize(const HWND hw);
        void Terminate();
        void AssignEmulatorWindow(const HWND hw);
        void StartInjectionThread();
        void EndInjectionThread();
        bool IsThreadRunning();
        [[nodiscard]] int GetConnectedDeviceCount() const;
        int SetPlayerHandle(PLAYERS player, int deviceclass, int phandle, int sechandle);
        std::shared_ptr<SDLDevice> GetFirstController();
        std::vector<std::shared_ptr<SDLDevice>> GetConnectedControllers();
        std::vector<SDLDevice> GetConnectedLeftJoycons();
        std::vector<SDLDevice> GetConnectedRightJoycons();
        static std::string GetButtonLabelForController(SDLDevice device, int buttonmask);
        static void SetDS4Color(SDLDevice dev, int color);
        void SetSPCJCNumber(SDLDevice dev, const int number);
        void CalibrateGyroscope(SDLDevice &sdldev);
        void CalibrateAllGyroscopes();
        std::vector<SDLDevice> GetConnectedDS4();
        std::string GetNameOfDevice(SDLDevice &device);
        void ReconnectControllers();
        void PauseInjection();
        void UnpauseInjection();
        int GetFirstButtonFromDevice(SDLDevice &jsd);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_SDLDRIVER_H
