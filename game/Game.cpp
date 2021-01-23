//
// Created by Robin on 1/18/2021.
//

#include "Game.h"

Game* Game::_instance = nullptr;

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
