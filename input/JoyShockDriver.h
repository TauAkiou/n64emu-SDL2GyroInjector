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
 * JoyShockDriver.h - Class for interfacting with JoyShockLibrary and operating core injection loop.
 */


#ifndef INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H
#define INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H

#include <vector>
#include <map>
#include <windows.h>
#include <ctime>
#include "../common/common.h"
#include "../settings/Settings.h"
#include "../JoyShockLibrary/JoyShockLibrary.h"
#include "../game/Game.h"

class JoyShockDriver {
    protected:
        static JoyShockDriver* instance;
        HANDLE _inputthread = nullptr;
        Settings* _settings = nullptr;
        ControlState* _cstateptr = nullptr;
        Game* _gameptr = nullptr;
        bool _terminatethread = true;
        int _devicecount = 0;
        int _windowactive = 1;
        bool _initialized = false;
        float calibration_time[ALLPLAYERS] = {};
        bool _looppaused = false;

        std::vector<JSDevice> *_devices = new std::vector<JSDevice>();
        HWND _emulatorwindow = nullptr;

        static DWORD WINAPI startinjectionloop(void* param);
        DWORD injectionloop();


    public:
        static JoyShockDriver* getInstance();
        JoyShockDriver();
        int Initialize(const HWND hw);
        void Terminate();
        void AssignEmulatorWindow(const HWND hw);
        void StartInjectionThread();
        void EndInjectionThread();
        bool IsThreadRunning();
        [[nodiscard]] int GetConnectedDeviceCount() const;
        int SetPlayerHandle(PLAYERS player, int deviceclass, int phandle, int sechandle);
        std::vector<JSDevice> GetConnectedFullControllers();
        std::vector<JSDevice> GetConnectedLeftJoycons();
        std::vector<JSDevice> GetConnectedRightJoycons();
        static std::string GetButtonLabelForController(JSDevice device, int buttonmask);
        static void SetDS4Color(JSDevice dev, int color);
        void SetSPCJCNumber(JSDevice dev, const int number);
        void CalibrateGyroscope(JSDevice &jsd);
        void CalibrateAllGyroscopes();
        std::vector<JSDevice> GetConnectedDS4();
        std::string GetNameOfDevice(JSDevice &device);
        void ReconnectControllers();
        void PauseInjection();
        void UnpauseInjection();

    static int GetFirstButtonFromDevice(JSDevice jsd);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H
