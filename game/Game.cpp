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

#include "Game.h"

void Game::Inject() {
    if(_loadedgame != nullptr)
        _loadedgame->Inject();
}

std::string Game::Name() {
    if(_loadedgame != nullptr)
        return _loadedgame->GetName();
    return "\0";
}

void Game::Quit() {
    if(_loadedgame != nullptr)
        _loadedgame->Quit();
    _loadedgame = nullptr;
}

int Game::Status() {
    if(_loadedgame != nullptr) {
        if(_loadedgame->Status())
            return 1;
        _loadedgame = nullptr;
    }
    for(GameDriver* drv : _gamedrivers) {
        if (drv != nullptr && drv->Status())
            _loadedgame = drv;
    }
    return (_loadedgame != nullptr);
}

Game * Game::GetInstance() {
    if(_instance == nullptr)
        _instance = new Game();
    return _instance;
}

void Game::AssignRamPtr(const unsigned char **ptr) {
        _link->SetRAMPointer(ptr);
}

void Game::AssignRomPtr(const unsigned char **ptr) {
        _link->SetROMPointer(ptr);
}
