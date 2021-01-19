//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H
#define INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H

#include "EmulatorLink.h"
#include <string>

class GameDriver {
protected:
    GameDriver* _driver = nullptr;
    EmulatorLink* _link = EmulatorLink::getInstance();
    std::string _name;
public:
    std::string GetName() {
        return std::string(_name);
    }
    virtual int Status() = 0;
    virtual void Inject() = 0;
    virtual void Quit() = 0;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_GAMEDRIVER_H
