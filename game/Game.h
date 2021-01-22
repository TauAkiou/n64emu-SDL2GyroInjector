//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_GAME_H
#define INC_1964_INPUT_JOYSHOCKCPP_GAME_H

#define FOV_MIN 40
#define FOV_MAX 120


#include <vector>
#include <string>
#include "driver/GameDriver.h"
#include "driver/Goldeneye.h"
#include "driver/PerfectDark.h"

class Game {
    protected:
        static Game* _instance;
        std::vector<GameDriver*> _gamedrivers = { new Goldeneye(), new PerfectDark() };
        GameDriver* _loadedgame = nullptr;
    public:
        static Game* GetInstance();
        int Status();
        std::string Name();
        void Inject();
        void Quit();
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_GAME_H
