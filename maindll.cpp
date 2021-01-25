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

#include "maindll.h"


MainDll * MainDll::GetInstance(HINSTANCE hinstance) {
    if(!MainDll::instance) {
        MainDll::instance = new MainDll(hinstance);
    }
    return MainDll::instance;
}

bool MainDll::IsConfigDialogOpen() const {
    return _configdialogisopen;
}

MainDll::MainDll(HINSTANCE hinstance) {
    // Start by initialzing all pointers.
    // TODO: Consider decoupling these objects from singletons.
    _settingsptr = Settings::GetInstance();
    _emuctrlptr = ControlState::GetInstance();
    _jsdptr = JoyShockDriver::getInstance();
    _gameptr = Game::GetInstance();
    _hinst = hinstance;

    wchar_t filepath[MAX_PATH] = {L'\0'};
    wchar_t directory[MAX_PATH] = {L'\0'};

    GetModuleFileNameW(_hinst, filepath, MAX_PATH);
    if (filepath != nullptr) {
        const wchar_t slash[] = L"\\";
        wchar_t *dllname;
        unsigned int dllnamelength = 19;
        dllname = wcspbrk(filepath, slash);
        while (dllname !=
               nullptr) { // find the last filename in full filepath and set filename length to dllnamelength (skip to slash every loop until last filename is found)
            dllnamelength = wcslen(dllname);
            dllname = wcspbrk(dllname + 1, slash);
        }
        wcsncpy(directory, filepath, wcslen(filepath) - dllnamelength + 1);
        directory[wcslen(filepath) - dllnamelength + 1] = L'\0'; // string needs terminator so add zero character to end
        _jsonfilepath.append(directory);
        _jsonfilepath.append(L"mouseinjector.json");
    }
}

bool MainDll::Initialize(const HWND hW) {
    auto count = _jsdptr->Initialize(hW);
    if(!count)
        return false;
    return true;
}

void MainDll::StartInjection() {
    _jsdptr->StartInjectionThread();
}

void MainDll::EndInjection() {
    _jsdptr->EndInjectionThread();
}

void MainDll::End() {
    _configdialogisopen = false;
    _ctrlptr = nullptr;
}

void MainDll::SetEmulatorOverclock(bool newoverclock) {
    _emuoverclock = newoverclock;
}

bool MainDll::InitiateControllers(HWND window, CONTROL *ptr) {
    _ctrlptr = ptr;

    Sleep(1000); // Sleep for a second to get everything initialized.

    if(!_jsdptr->Initialize(window)) {
        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            _settingsptr->SetAssignmentForPlayer(static_cast<PLAYERS>(player), {DISCONNECTED,
                                                                                {-1, None},
                                                                                {-1, None}});
        }
        UpdateControllerStatus();
        return false;
    }
        // Set up configuration objects.


    // Use the first detected FULL controller type.

    auto ctrllst_full = _jsdptr->GetConnectedFullControllers();
    auto ctrllst_ljc = _jsdptr->GetConnectedLeftJoycons();
    auto ctrllst_rjc = _jsdptr->GetConnectedRightJoycons();

    /*
    if(ctrllist.empty()) {
        // No controller, just in case here
        player1prof.SETTINGS[CONFIG] = DISABLED;
        return false;
    }
     */

    Assignment player1asgn = {FULLCONTROLLER, ctrllst_full.front(),
                             {-1, None}};
    Assignment player2asgn { DISCONNECTED, ctrllst_full.back(),
                             {-1, None}};

    PROFILE player1prof = {};
    PROFILE player2prof = {};
    
    player1prof.FreeAiming = true;
    player1prof.StickMode = FLICK;
    player1prof.UseStickToAim = false;
    player1prof.DS4Color = 0x0000FF;
    player1prof.AimStickSensitivity = {23000, 23000};
    player1prof.GyroscopeSensitivity = {400, 400};
    player1prof.Crosshair = 1;
    player1prof.PitchInverted = false;
    player1prof.CrouchToggle = true;
    player1prof.GoldeneyeAimMode = true;
    player1prof.PerfectDarkAimMode = true;
    player1prof.AimstickDeadzone = {0.2, 0.2};


    player1prof.BUTTONPRIM[FIRE] = JSMASK_ZR;
    player1prof.BUTTONPRIM[AIM] = JSMASK_ZL;
    player1prof.BUTTONPRIM[ACCEPT] = JSMASK_L;
    player1prof.BUTTONPRIM[CANCEL] = JSMASK_R;
    player1prof.BUTTONPRIM[START] = JSMASK_PLUS;
    player1prof.BUTTONPRIM[CROUCH] = JSMASK_E;
    player1prof.BUTTONPRIM[KNEEL] = JSMASK_S;
    player1prof.BUTTONPRIM[PREVIOUSWEAPON] = JSMASK_W;
    player1prof.BUTTONPRIM[NEXTWEAPON] = JSMASK_N;
    player1prof.BUTTONPRIM[UP] = JSMASK_UP;
    player1prof.BUTTONPRIM[DOWN] = JSMASK_DOWN;
    player1prof.BUTTONPRIM[LEFT]= JSMASK_LEFT;
    player1prof.BUTTONPRIM[RIGHT] = JSMASK_RIGHT;
    player1prof.BUTTONPRIM[RESETGYRO] = JSMASK_MINUS;
    player1prof.CalibrationButton = JSMASK_CAPTURE;

    _settingsptr->SetProfileForPlayer(PLAYER1, player1prof);
    _settingsptr->SetAssignmentForPlayer(PLAYER1, player1asgn);

    player2prof.FreeAiming = true;
    player2prof.StickMode = FLICK;
    player2prof.UseStickToAim = false;
    player2prof.DS4Color = 0x0000FF;
    player2prof.AimStickSensitivity = {23000, 23000};
    player2prof.GyroscopeSensitivity = {400, 400};
    player2prof.Crosshair = 1;
    player2prof.PitchInverted = false;
    player2prof.CrouchToggle = true;
    player2prof.GoldeneyeAimMode = true;
    player2prof.PerfectDarkAimMode = true;

    player2prof.BUTTONPRIM[FIRE] = JSMASK_ZR;
    player2prof.BUTTONPRIM[AIM] = JSMASK_ZL;
    player2prof.BUTTONPRIM[ACCEPT] = JSMASK_L;
    player2prof.BUTTONPRIM[CANCEL] = JSMASK_R;
    player2prof.BUTTONPRIM[START] = JSMASK_PLUS;
    player2prof.BUTTONPRIM[CROUCH] = JSMASK_E;
    player2prof.BUTTONPRIM[KNEEL] = JSMASK_S;
    player2prof.BUTTONPRIM[PREVIOUSWEAPON] = JSMASK_W;
    player2prof.BUTTONPRIM[NEXTWEAPON] = JSMASK_N;
    player2prof.BUTTONPRIM[UP] = JSMASK_UP;
    player2prof.BUTTONPRIM[DOWN] = JSMASK_DOWN;
    player2prof.BUTTONPRIM[LEFT]= JSMASK_LEFT;
    player2prof.BUTTONPRIM[RIGHT] = JSMASK_RIGHT;
    player2prof.BUTTONPRIM[RESETGYRO] = JSMASK_MINUS;
    player2prof.CalibrationButton = JSMASK_CAPTURE;

    _settingsptr->SetProfileForPlayer(PLAYER2, player2prof);
    _settingsptr->SetAssignmentForPlayer(PLAYER2, player2asgn);

    _settingsptr->SetShowGoldeneyeCrosshair(true);
    _settingsptr->SetFovOverride(90);


    /*
    _settingsptr->Profile[PLAYER2].ControllerMode = 0;
    _settingsptr->Profile[PLAYER2].AssignedDevicePrimary = ctrllst_full.back();
    //player1prof.AssignedDeviceSecondary = ctrllst_rjc.front();
    //player1prof.AssignedDevicePrimary = ctrllst_ljc.front();

    _settingsptr->Profile[PLAYER2].FreeAiming = true;
    _settingsptr->Profile[PLAYER2].StickMode = FLICK;
    _settingsptr->Profile[PLAYER2].UseStickToAim = false;
    _settingsptr->Profile[PLAYER2].DS4Color = 0x0000FF;

    _settingsptr->Profile[PLAYER2].AimStickSensitivity = {23000, 23000};
    _settingsptr->Profile[PLAYER2].GyroscopeSensitivity = {400, 400};
    _settingsptr->Profile[PLAYER2].AimstickDeadzone = {0.10, 0.10};

    _settingsptr->Profile[PLAYER2].Crosshair = 1;
    _settingsptr->Profile[PLAYER2].PitchInverted = false;
    _settingsptr->Profile[PLAYER2].CrouchToggle = true;
    _settingsptr->Profile[PLAYER2].GoldeneyeAimMode = true;
    _settingsptr->Profile[PLAYER2].PerfectDarkAimMode = true;

    _settingsptr->Profile[PLAYER2].BUTTONPRIM[FIRE] = JSMASK_ZR;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[AIM] = JSMASK_ZL;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[ACCEPT] = JSMASK_L;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[CANCEL] = JSMASK_R;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[START] = JSMASK_PLUS;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[CROUCH] = JSMASK_E;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[KNEEL] = JSMASK_S;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[PREVIOUSWEAPON] = JSMASK_W;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[NEXTWEAPON] = JSMASK_N;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[UP] = JSMASK_UP;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[DOWN] = JSMASK_DOWN;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[LEFT]= JSMASK_LEFT;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[RIGHT] = JSMASK_RIGHT;
    _settingsptr->Profile[PLAYER2].BUTTONPRIM[RESETGYRO] = JSMASK_MINUS;
    _settingsptr->Profile[PLAYER2].CalibrationButton = JSMASK_CAPTURE;

    _settingsptr->SetShowGoldeneyeCrosshair(true);
    _settingsptr->SetFovOverride(90);
     */

    UpdateControllerStatus();
    return true;
}

void MainDll::UpdateControllerStatus() {
    if(_ctrlptr == nullptr)
        return;

    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        auto asgn = _settingsptr->GetAssignmentForPlayer(static_cast<PLAYERS>(player));
        _ctrlptr[player].Present = (asgn.ControllerMode) != DISCONNECTED;
        _ctrlptr[player].RawData = false;
        _ctrlptr[player].Plugin = player == PLAYER1 ? PLUGIN_MEMPACK : PLUGIN_NONE;
    }
}

void MainDll::HandleConfigWindow(HWND hW) {
    PluginUi::OpenDialogWindow(hW, _hinst);
}

