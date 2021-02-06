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

#include <memory>


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
    //_emuctrlptr = ControlState::GetInstance();
    _jsdptr = SdlDriver::getInstance();
    //_gameptr = Game::GetInstance();
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

MainDll::~MainDll() {
    // Delete all pointers.
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
    EndInjection();
    delete _settingsptr;
    delete _jsdptr;
    _configdialogisopen = false;
    _ctrlptr = nullptr;
}

void MainDll::SetEmulatorOverclock(bool newoverclock) {
    _settingsptr->SetIfEmulatorOverclocked(newoverclock);
}

bool MainDll::InitiateControllers(HWND window, CONTROL *ptr) {
    _ctrlptr = ptr;

    Sleep(1000); // Sleep for a second to get everything initialized.

    if(!_jsdptr->Initialize(window)) {
        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            _settingsptr->SetAssignmentForPlayer(static_cast<PLAYERS>(player), {DISCONNECTED,
                                                                                nullptr,nullptr});
        }
        UpdateControllerStatus();
        return false;
    }
        // Set up configuration objects.


    // Use the first detected FULL controller type.

    auto ctrllst_full = _jsdptr->GetConnectedControllers();
    auto ctrllst_ljc = _jsdptr->GetConnectedLeftJoycons();
    auto ctrllst_rjc = _jsdptr->GetConnectedRightJoycons();

    /*
    if(ctrllist.empty()) {
        // No controller, just in case here
        player1prof.SETTINGS[CONFIG] = DISABLED;
        return false;
    }
     */

    Assignment player1asgn, player2asgn, player3asgn, player4asgn;

    if(!ctrllst_full.empty()) {
        player1asgn = {FULLCONTROLLER, ctrllst_full.front(),
                                  nullptr};
        player2asgn = {DISCONNECTED, nullptr,
                               nullptr};
    }

    PROFILE player1prof, player2prof, player3prof, player4prof;


    if(player1asgn.PrimaryDevice != nullptr) {
        player1asgn.PrimaryDevice->AssignPlayerIndex(0);
    }
    if(player2asgn.PrimaryDevice != nullptr) {
        player1asgn.PrimaryDevice->AssignPlayerIndex(1);

    }
    if(player3asgn.PrimaryDevice != nullptr) {
        player1asgn.PrimaryDevice->AssignPlayerIndex(2);

    }
    if(player4asgn.PrimaryDevice != nullptr) {
        player1asgn.PrimaryDevice->AssignPlayerIndex(3);

    }


    player1prof.DS4Color = 0x0000FF;
    player2prof.DS4Color = 0xFF0000;
    player3prof.DS4Color = 0x00FF00;
    player4prof.DS4Color = 0xFFFF00;

    _settingsptr->SetProfileForPlayer(PLAYER1, player1prof);
    _settingsptr->SetAssignmentForPlayer(PLAYER1, player1asgn);

    _settingsptr->SetProfileForPlayer(PLAYER2, player2prof);
    _settingsptr->SetAssignmentForPlayer(PLAYER2, player2asgn);

    _settingsptr->SetProfileForPlayer(PLAYER3, player3prof);
    _settingsptr->SetAssignmentForPlayer(PLAYER3, player3asgn);

    _settingsptr->SetProfileForPlayer(PLAYER4, player4prof);
    _settingsptr->SetAssignmentForPlayer(PLAYER4, player4asgn);

    _settingsptr->SetShowGoldeneyeCrosshair(true);
    _settingsptr->SetFovOverride(90);

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


int MainDll::HandleConfigWindow(int argc, char* argv[]) {
    std::unique_ptr<QApplication> pQApp;
    QCoreApplication* pApp = QCoreApplication::instance();
    // Stop the injection thread while we are in settings.
    if(_romloaded && _jsdptr->IsThreadRunning()) {
        _jsdptr->EndInjectionThread();
    }

    if(pApp == nullptr) {
        pQApp = std::make_unique<QApplication>(argc, argv);
        pApp = pQApp.get();
    }

    ConfigDialog cfgdlg;
    cfgdlg.show();
    int res = pQApp ? pQApp->exec() : cfgdlg.exec();

    UpdateControllerStatus();
    if(_romloaded && !_jsdptr->IsThreadRunning()) {
        _jsdptr->StartInjectionThread();
    }
    return 0;
}

bool MainDll::isRomloaded() const {
    return _romloaded;
}

void MainDll::setRomloaded(bool romloaded) {
    _romloaded = romloaded;
}


