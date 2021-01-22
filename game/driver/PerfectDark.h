//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H
#define INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H

#include "../../input/InputHandler.h"
#include "GameDriver.h"

#define PD_ResetCrouchToggle(X) safetocrouch[X] = 1, safetostand[X] = 0, crouchstance[X] = 2 // reset crouch toggle bind
#define PD_ResetXYStick(X) xstick[X] = 0, ystick[X] = 0, usingstick[X] = 0 // reset x/y stick array
#define PD_ResetRadialMenuBtns(X) for(int direction = 0; direction < 4; direction++) radialmenudirection[X][direction] = 0 // reset direction buttons
#define PD_WITHINRANGE(param) ((_link.rdramptr != 0) && ((param & 0xFF800000U) == 0x80000000U))


class PerfectDark : public GameDriver {
private:
    std::string _name = "Perfect Dark";
    unsigned int playerbase[4] = {0};
    InputHandler _ihandler;
    int xstick[4] = {0};
    int ystick[4] = {0};
    int usingstick[4] = {0};
    int xmenu[4] = {0};
    int ymenu[4] = {0};
    int radialmenudirection[4][4] = {{0}, {0}, {0}, {0}};
    int safetocrouch[4] = { 1, 1, 1, 1 };
    int safetostand[4] = {0};
    int crouchstance[4] = {0};
    float crosshairposx[4]{};
    float crosshairposy[4]{};
    float aimx[4]{};
    float aimy[4]{};
    int gunrcenter[4]{};
    int gunlcenter[4]{};

    void _crouch(const int player);
    void _camspyslayer(const int player, const int camspyflag, const float sensitivityx, const float sensitivityy);
    void _aimmode(const int player, const int aimingflag, const float fov, const float basefov);
    void _radialmenunav(const int player);
    void _resetgyro();
    void _controller();
    void _injecthacks();

public:
    PerfectDark() = default;
    int Status() override;
    void Inject() override;
    void Quit() override;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_PERFECTDARK_H
