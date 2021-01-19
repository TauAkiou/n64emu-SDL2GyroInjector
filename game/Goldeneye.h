//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H
#define INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H

#define GUNAIMLIMIT 6.879164696 // 0x40DC221E
#define CROSSHAIRLIMIT 5.159373283 // 0x40A51996
#define TANKXROTATIONLIMIT 6.283185005 // 0x40C90FDA
#define PI 3.1415927 // 0x40490FDB
// GOLDENEYE ADDRESSES - OFFSET ADDRESSES BELOW (REQUIRES PLAYERBASE TO USE)
#define GE_stanceflag 0x800D2FFC - 0x800D2F60
#define GE_deathflag 0x800D3038 - 0x800D2F60
#define GE_camx 0x800D30A8 - 0x800D2F60
#define GE_camy 0x800D30B8 - 0x800D2F60
#define GE_fov 0x800D4124 - 0x800D2F60
#define GE_crosshairx 0x800D3F50 - 0x800D2F60
#define GE_crosshairy 0x800D3F54 - 0x800D2F60
#define GE_watch 0x800D3128 - 0x800D2F60
#define GE_gunx 0x800D3F64 - 0x800D2F60
#define GE_guny 0x800D3F68 - 0x800D2F60
#define GE_aimingflag 0x800D3084 - 0x800D2F60
#define GE_currentweapon 0x800D37D0 - 0x800D2F60
#define GE_multipausemenu 0x800A9D24 - 0x800A7360
// STATIC ADDRESSES BELOW
#define BONDDATA(X) (unsigned int)EMU_ReadInt(0x80079EE0 + (X * 0x4)) // player pointer address (0x4 offset for each players)
#define GE_camera 0x80036494 // camera flag (0 = multiplayer, 1 = map overview, 2 = start flyby, 3 = in flyby, 4 = player in control, 5 = trigger restart map)
#define GE_exit 0x800364B0 // exit flag (0 = disable controls, 1 = enable controls)
#define GE_pause 0x80048370 // pause flag (1 = GE is paused)
#define GE_menupage 0x8002A8C0 // menu page id
#define GE_menux 0x8002A908 // crosshair menu cursor x axis
#define GE_menuy 0x8002A90C // crosshair menu cursor y axis
#define GE_tankxrot 0x80036484 // tank x rotation
#define GE_tankflag 0x80036448 // tank flag (0 = walking, 1 = in-tank)
#define GE_matchended 0x8008C700 // multiplayer match flag
#define GE_defaultratio 0x80055264 // 16:9 ratio default
#define GE_defaultfov 0x000B78BC // field of view default (rom)
#define GE_defaultfovinit 0x000CF838 // field of view init value (rom)
#define GE_defaultfovzoom 0x000B78DC // field of view default for zoom (rom)
#define GE_defaultzoomspeed 0x8004F1A8 // default zoom speed
#define GE_showcrosshair 0x0009F128 // show crosshair code (rom)
#define GE_crosshairimage 0x0029DE8C // crosshair image (rom)
#define GE_introcounter 0x8002A8CC // counter for intro
#define GE_seenintroflag 0x8002A930 // seen intro flag
#define GE_controlstyle 0x000D98FC // instruction reads the current controller style (rom)
#define GE_reversepitch 0x000D9970 // instruction reads the current reverse pitch option (rom)
#define GE_pickupyaxisthreshold 0x800532E0 // y axis threshold on picking up weapons
#define GE_weaponypos 0x8003249C // y axis position for view models
#define GE_weaponzpos (GE_weaponypos + 4) // z axis position for view models
#define GE_ResetCrouchToggle(X) safetocrouch[X] = 1, safetostand[X] = 0, crouchstance[X] = 0 // reset crouch toggle bind



#include "GameDriver.h"
#include "EmulatorLink.h"
#include "../maindll.h"

class Goldeneye : public GameDriver {
private:
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
    Goldeneye();
    int Status() override;
    void Inject() override;
    void Quit() override;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_GOLDENEYE_H
