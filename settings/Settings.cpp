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

namespace nl = nlohmann;

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

js_settings::PROFILE Settings::GetProfileForPlayer(enum PLAYERS player) {
    return Profile[player];
}

bool Settings::GetIfEmulatorOverclocked() const {
    return EmulatorSettings.IsOverclocked;
}

void Settings::SetProfileForPlayer(enum PLAYERS player, js_settings::PROFILE profile) {
    Profile[player] = std::move(profile);
}

bool Settings::GetShowGoldeneyeCrosshair() const {
    return EmulatorSettings.GoldeneyeCrosshair;
}

void Settings::SetShowGoldeneyeCrosshair(bool val) {
    EmulatorSettings.GoldeneyeCrosshair = val;
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
    return EmulatorSettings.FovOverride;
}

void Settings::SetFovOverride(int fov) {
    EmulatorSettings.FovOverride = fov;
}

vec2<int> Settings::GetOverrideRatio() {
    return EmulatorSettings.RatioOverride;
}

void Settings::SetOverrideRatio(const vec2<int>& overrideratio) {
    EmulatorSettings.RatioOverride.x = overrideratio.x;
    EmulatorSettings.RatioOverride.y = overrideratio.y;
}

void Settings::SetAssignmentForPlayer(enum PLAYERS player, Assignment asgn) {
    ControllerAssignments[player] = asgn;
}

void Settings::SetIfEmulatorOverclocked(bool isoverclocked) {
    EmulatorSettings.IsOverclocked = isoverclocked;
}

int Settings::LoadConfigFile() {
    // All settings are stored in this object, so we don't need parameters - this simply commits settings changes.
    std::filesystem::path filepath = _jsonfilepath;
    auto fileexists = std::filesystem::exists(filepath);


    try {
        if (fileexists) {

            std::ifstream json_stream((std::filesystem::path(_jsonfilepath)));
            nl::json json_parsed;

            json_stream >> json_parsed;

            EmulatorSettings = json_parsed.at("emu").get<js_settings::EMUSETTINGS>();

            // start by reading emulator settings


            // profiles
            for (int x = 0; x < ALLPLAYERS; x++) {
                js_settings::PROFILE loading_profile;

                nl::json json_profile_for_player = json_parsed.at("profile").at(std::to_string(x + 1));

                loading_profile = json_profile_for_player.get<js_settings::PROFILE>();

                Profile[x] = loading_profile;

            }
        } else {

            // Create a new configuration file with all default values.
            js_settings::PROFILE defprofile;

            Profile[0] = defprofile;
            Profile[1] = defprofile;
            Profile[2] = defprofile;
            Profile[3] = defprofile;

            SaveConfigFile();
        }
    }
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}

int Settings::SaveConfigFile() {
    nl::json config_json;

    int prof = 1;
    for(auto psave : Profile) {
        config_json["profile"][std::to_string(prof)] = psave;
        prof++;
    }

    // Emulator settings.
    config_json["emu"] = EmulatorSettings;

    std::filesystem::path fpath = _jsonfilepath;
    std::ofstream ofs(fpath, std::ofstream::trunc);

    ofs << std::setw(1) << config_json << std::endl;
    ofs.close();

    return 0;
}

ControlState* ControlState::GetInstance() {
    if(_instance == nullptr)
        _instance = new ControlState();
    return _instance;
}



