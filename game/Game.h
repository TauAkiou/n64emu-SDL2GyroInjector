//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_GAME_H
#define INC_1964_INPUT_JOYSHOCKCPP_GAME_H

#include <vector>
#include <string>
#include "Goldeneye.h"
#include "PerfectDark.h"
#include "GameDriver.h"


class Game {
    protected:
        Game* _instance = nullptr;
        std::vector<GameDriver*> _gamedrivers { new Goldeneye() };
        GameDriver* _loadedgame = nullptr;
    public:
        static Game* getInstance();
        int Status();
        const std::string Name();
        void Inject();
        void Quit();
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_GAME_H
