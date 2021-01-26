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

/*
 * maindll.h - Entrypoint class used to interact with and set up other object.
 */

#ifndef INC_1964_INPUT_JOYSHOCKCPP_MAINDLL_H
#define INC_1964_INPUT_JOYSHOCKCPP_MAINDLL_H

#include <iostream>
#include <cmath>
#include <windows.h>
#include <commctrl.h>
#include "common/common.h"
#include "plugin.h"
#include "game/EmulatorLink.h"
#include "input/JoyShockDriver.h"
#include "settings/Settings.h"
#include "game/Game.h"
#include "ui/ConfigDialog.h"

#define DLLEXPORT __declspec(dllexport)
#define CALL __cdecl

class MainDll {
protected:
    inline static MainDll* instance;
    HINSTANCE _hinst = nullptr;
    std::wstring _jsonfilepath = {L'\0'};
    bool _guibusy = true;
    int _changeratio = 0;
    bool _emuoverclock = true;

    bool _configdialogisopen = false;
    bool _threadrunning = false;
    CONTROL *_ctrlptr = nullptr;

    // Pointers to objects.
    Settings* _settingsptr = nullptr;
    ControlState* _emuctrlptr = nullptr;
    JoyShockDriver* _jsdptr = nullptr;
    Game* _gameptr = nullptr;

    explicit MainDll(HINSTANCE hinstance);
    bool Initialize(const HWND hW);
    //~MainDll();

public:
    static MainDll* GetInstance(HINSTANCE hinstance = nullptr);
    void End();
    [[nodiscard]] bool IsConfigDialogOpen() const;
    void SetEmulatorOverclock(bool newoverclock);
    bool InitiateControllers(HWND window, CONTROL *ptr);
    void UpdateControllerStatus();
    void StartInjection();
    void EndInjection();

    int HandleConfigWindow(int argc, char **argv);
};



static const std::string JsonFilePathDefault = ".\\plugin\\gyroinjector.json";

#endif //INC_1964_INPUT_JOYSHOCKCPP_MAINDLL_H