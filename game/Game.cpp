//
// Created by Robin on 1/18/2021.
//

#include "Game.h"

void Game::Inject() {}

const std::string Game::Name() {}

void Game::Quit() {}

int Game::Status() {
    if(_loadedgame != nullptr) {
        if(_loadedgame->Status())
            return 1;
        _loadedgame = nullptr;
    }
    const Game* game;
    for(GameDriver* drv : _gamedrivers) {
        if (drv != nullptr && drv->Status())
            _loadedgame = drv;
    }
    return (_loadedgame != nullptr);
}

Game * Game::getInstance() {
    if(_instance == nullptr)
        _instance = new Game();
    return _instance;
}