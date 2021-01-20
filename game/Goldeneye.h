//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H
#define INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H

#include "GameDriver.h"

class Goldeneye : public GameDriver {
private:
    Controls* _cfgptr = Controls::getInstance();
    std::string _name = "GoldenEye 007";
    unsigned int playerbase[4] = {0};
    int safetocrouch[4] = { 1, 1, 1, 1 };
    int safetostand[4] = {0};
    int crouchstance[4] = {0};
    float crosshairposx[4];
    float crosshairposy[4];
    float aimx[4];
    float aimy[4];
    void _crouch(const int player);
    void _aimmode(const int player, const int aimingflag, const float fov, const float basefov);
    void _resetgyro();
    void _controller();
    void _injecthacks();

public:
    Goldeneye() = default;
    int Status() override;
    void Inject() override;
    void Quit() override;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H
