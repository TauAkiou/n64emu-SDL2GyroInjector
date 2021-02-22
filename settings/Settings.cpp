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

#include "Settings.h"
#include <utility>

Settings* Settings::GetInstance(HINSTANCE hinstance) {
    if(_instance == nullptr) {
        _instance = new Settings(hinstance);
    }
    return _instance;
}

Settings::Settings(HINSTANCE _hinst) {
    try {
        if(_hinst == nullptr) throw std::exception();
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
            directory[wcslen(filepath) - dllnamelength +
                      1] = L'\0'; // string needs terminator so add zero character to end
            _jsonfilepath.append(directory);
            _jsonfilepath.append(L"gyroinjector.json");
        }
    }
    catch(std::exception &exc) {
        throw;
    }
}

PROFILE Settings::GetProfileForPlayer(enum PLAYERS player) {
    return Profile[player];
}

bool Settings::GetIfEmulatorOverclocked() const {
    return EmulatorOverclocked;
}

void Settings::SetProfileForPlayer(enum PLAYERS player, PROFILE profile) {
    Profile[player] = std::move(profile);
}

bool Settings::GetShowGoldeneyeCrosshair() const {
    return ShowGoldeneyeCrosshair;
}

void Settings::SetShowGoldeneyeCrosshair(bool val) {
    ShowGoldeneyeCrosshair = val;
}

bool Settings::GetIfPlayerIsConnected(enum PLAYERS player) {
    switch(ControllerAssignments[player].ControllerMode) {
        default:
        case DISCONNECTED:
            return false;
        case FULLCONTROLLER:
        case JOYCONS:
            return true;
    }
}

Assignment Settings::GetAssignmentForPlayer(enum PLAYERS player) {
    return ControllerAssignments[player];
}


int Settings::GetFovOverride() const {
    return FovOverride;
}

void Settings::SetFovOverride(int fov) {
    FovOverride = fov;
}

vec2<int> Settings::GetOverrideRatio() {
    return _overrideRatio;
}

void Settings::SetOverrideRatio(const vec2<int>& overrideratio) {
    _overrideRatio = overrideratio;
}

void Settings::SetAssignmentForPlayer(enum PLAYERS player, Assignment asgn) {
    ControllerAssignments[player] = asgn;
}

void Settings::SetIfEmulatorOverclocked(bool isoverclocked) {
    EmulatorOverclocked = isoverclocked;
}

ControlState* ControlState::GetInstance() {
    if(_instance == nullptr)
        _instance = new ControlState();
    return _instance;
}


