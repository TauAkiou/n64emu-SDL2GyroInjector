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
    _settingsptr = Settings::GetInstance(hinstance);
    //_emuctrlptr = ControlState::GetInstance();
    _jsdptr = SdlDriver::getInstance();
    //_gameptr = Game::GetInstance();
    _hinst = hinstance;
}

MainDll::~MainDll() {
    // Delete all pointers.
}

void MainDll::LoadConfig() {
    // time to start handling the configuration file... the big one one would guess
};

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

    // Load the configuration file.
    _settingsptr->LoadConfigFile();

    if(!_jsdptr->Initialize(window)) {
        for(int player = PLAYER1; player < ALLPLAYERS; player++) {
            _settingsptr->SetAssignmentForPlayer(static_cast<PLAYERS>(player), {DISCONNECTED,
                                                                                nullptr,nullptr});
        }
        UpdateControllerStatus();
        return false;
    }

    auto ctrllst_full = _jsdptr->GetConnectedControllers();
    auto ctrllst_ljc = _jsdptr->GetConnectedLeftJoycons();
    auto ctrllst_rjc = _jsdptr->GetConnectedRightJoycons();

    Assignment player1asgn, player2asgn, player3asgn, player4asgn;

    if(!ctrllst_full.empty()) {
        player1asgn = {FULLCONTROLLER, ctrllst_full.front(),
                                  nullptr};
        player2asgn = {DISCONNECTED, nullptr,
                               nullptr};
    }


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


