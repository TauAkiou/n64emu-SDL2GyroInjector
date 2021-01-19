//
// Created by Robin on 1/18/2021.
//

#include "Goldeneye.h"

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
#define BONDDATA(X) (unsigned int)_link->ReadInt(0x80079EE0 + (X * 0x4)) // player pointer address (0x4 offset for each players)
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

int Goldeneye::Status() {
    const int ge_camera = _link->ReadInt(GE_camera), ge_page = _link->ReadInt(GE_menupage), ge_pause = _link->ReadInt(GE_pause), ge_exit = _link->ReadInt(GE_exit);
    const float ge_crosshairx = _link->ReadFloat(GE_menux), ge_crosshairy = _link->ReadFloat(GE_menuy);
    return (ge_camera >= 0 && ge_camera <= 10 && ge_page >= -1 && ge_page <= 25 && ge_pause >= 0 && ge_pause <= 1 && ge_exit >= 0 && ge_exit <= 1 && ge_crosshairx >= 20 && ge_crosshairx <= 420 && ge_crosshairy >= 20 && ge_crosshairy <= 310); // if GoldenEye 007 is current game
}

void Goldeneye::Inject() {
    if(_link->ReadInt(GE_menupage) < 1) // hacks can only be injected at boot sequence before code blocks are cached, so inject until the main menu
        _injecthacks();
    const int camera = _link->ReadInt(GE_camera);
    const int exit = _link->ReadInt(GE_exit);
    const int pause = _link->ReadInt(GE_pause);
    const int menupage = _link->ReadInt(GE_menupage);
    const int tankflag = _link->ReadInt(GE_tankflag);
    const int mproundend = _link->ReadInt(GE_matchended);
    for(int player = PLAYER1; player < ALLPLAYERS; player++)
    {
        if(_cfgptr->Profile[player].SETTINGS[CONFIG] == DISABLED) // bypass disabled players
            continue;
        playerbase[player] = BONDDATA(player);
        const int dead = _link->ReadInt(playerbase[player] + GE_deathflag);
        const int watch = _link->ReadInt(playerbase[player] + GE_watch);
        const int aimingflag = _link->ReadInt(playerbase[player] + GE_aimingflag);
        const int mppausemenu = _link->ReadInt(playerbase[player] + GE_multipausemenu);
        const int cursoraimingflag = _cfgptr->Profile[player].SETTINGS[GEAIMMODE] && aimingflag;
        const float fov = _link->ReadFloat(playerbase[player] + GE_fov);
        const float basefov = fov > 60.0f ? (float)OVERRIDEFOV : 60.0f;
        //const float mouseaccel = _cfgptr->Profile[player].SETTINGS[ACCELERATION] ? sqrt(_cfgptr->Device[player].XPOS * _cfgptr->Device[player].XPOS + _cfgptr->Device[player].YPOS * _cfgptr->Device[player].YPOS) / TICKRATE / 12.0f * _cfgptr->Profile[player].SETTINGS[ACCELERATION] : 0;
        const float sensitivity_stick_x = _cfgptr->Profile[player].SETTINGS[STICKSENSITIVITYX] / 40.0f;// * fmax(mouseaccel, 1);
        const float sensitivity_stick_y = _cfgptr->Profile[player].SETTINGS[STICKSENSITIVITYY] / 40.0f; // * fmax(mouseaccel, 1);
        const float sensitivity_gyro_x = _cfgptr->Profile[player].SETTINGS[GYROSENSITIVITYX] / 40.0f;
        const float sensitivity_gyro_y = _cfgptr->Profile[player].SETTINGS[GYROSENSITIVITYY] / 40.0f; // fmax(mouseaccel, 1);

        const float gunsensitivity_stick_x = sensitivity_stick_x * (_cfgptr->Profile[player].SETTINGS[CROSSHAIR] / 2.5f);
        const float gunsensitivity_stick_y = sensitivity_stick_y * (_cfgptr->Profile[player].SETTINGS[CROSSHAIR] / 2.5f);
        const float gunsensitivity_gyro_x = sensitivity_gyro_x * (_cfgptr->Profile[player].SETTINGS[CROSSHAIR] / 2.5f);
        const float gunsensitivity_gyro_y = sensitivity_gyro_y * (_cfgptr->Profile[player].SETTINGS[CROSSHAIR] / 2.5f);

        float camx = _link->ReadFloat(playerbase[player] + GE_camx), camy = _link->ReadFloat(playerbase[player] + GE_camy);
        if(camx >= 0 && camx <= 360 && camy >= -90 && camy <= 90 && fov >= 1 && fov <= FOV_MAX && dead == 0 && watch == 0 && pause == 0 && (camera == 4 || camera == 0) && exit == 1 && menupage == 11 && !mproundend && !mppausemenu) // if safe to inject
        {
            _aimmode(player, cursoraimingflag, fov, basefov);
            if(!tankflag) // player is on foot
            {
                _crouch(player); // only allow crouching if player is not in tank
                if(!cursoraimingflag) { // if not aiming (or geaimmode is off)
                    camx += _cfgptr->Device[player].RSTICKX / 10.0f * sensitivity_stick_x *
                            (fov / basefov); // regular mouselook calculation
                    camx += _cfgptr->Device[player].GYROX / 10.0f * sensitivity_gyro_x * _cfgptr->Device[player].DELTATIME * (fov / basefov);
                }
                else
                    camx += aimx[player] * (fov / basefov); // scroll screen with aimx/aimy
                while(camx < 0)
                    camx += 360;
                while(camx >= 360)
                    camx -= 360;
                _link->WriteFloat(playerbase[player] + GE_camx, camx);
            }
            else // player is in tank
            {
                GE_ResetCrouchToggle(player); // reset crouch toggle if in tank
                float tankx = _link->ReadFloat(GE_tankxrot);
                if(!cursoraimingflag || _link->ReadInt(playerbase[player] + GE_currentweapon) == 32) { // if not aiming (or geaimmode is off) or player is driving tank with tank equipped as weapon, then use regular mouselook calculation
                    tankx += _cfgptr->Device[player].RSTICKX / 10.0f * sensitivity_stick_x / (360 / TANKXROTATIONLIMIT * 2.5) *
                             (fov / basefov);
                    tankx += _cfgptr->Device[player].GYROX / 10.0f * sensitivity_gyro_x / (360 / TANKXROTATIONLIMIT * 2.5) *
                             _cfgptr->Device[player].DELTATIME * (fov / basefov);
                }
                else
                    tankx += aimx[player] / (360 / TANKXROTATIONLIMIT * 2.5) * (fov / basefov);
                while(tankx < 0)
                    tankx += TANKXROTATIONLIMIT;
                while(tankx >= TANKXROTATIONLIMIT)
                    tankx -= TANKXROTATIONLIMIT;
                _link->WriteFloat(GE_tankxrot, tankx);
            }
            if(!cursoraimingflag) {
                camy += (!_cfgptr->Profile[player].SETTINGS[INVERTPITCH] ? -_cfgptr->Device[player].AIMSTICKY : _cfgptr->Device[player].AIMSTICKY) /
                        10.0f * sensitivity_stick_y * (fov / basefov);
                camy += (!_cfgptr->Profile[player].SETTINGS[INVERTPITCH] ? -_cfgptr->Device[player].GYROY : _cfgptr->Device[player].GYROY) /
                        10.0f * sensitivity_gyro_y * _cfgptr->DeltaTime * (fov / basefov);
            }
            else
                camy += -aimy[player] * (fov / basefov);
            camy = ClampFloat(camy, tankflag ? -20 : -90, 90); // tank limits player from looking down -20
            _link->WriteFloat(playerbase[player] + GE_camy, camy);
            if(_cfgptr->Profile[player].SETTINGS[CROSSHAIR] && !cursoraimingflag) // if crosshair movement is enabled and player isn't aiming (don't calculate weapon movement while the player is in aim mode)
            {
                if(!tankflag)
                {
                    float gunx = _link->ReadFloat(playerbase[player] + GE_gunx), crosshairx = _link->ReadFloat(playerbase[player] + GE_crosshairx); // after camera x and y have been calculated and injected, calculate the gun/crosshair movement
                    gunx += _cfgptr->Device[player].RSTICKX / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / basefov) * 0.019f;
                    gunx += _cfgptr->Device[player].GYROX / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * _cfgptr->Device[player].DELTATIME * (fov / basefov) * 0.019f;
                    crosshairx += _cfgptr->Device[player].RSTICKX / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / 4 / (basefov / 4)) * 0.01912f / RATIOFACTOR;
                    crosshairx += _cfgptr->Device[player].GYROX / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * (fov / 4 / (basefov / 4)) * _cfgptr->Device[player].DELTATIME * 0.01912f / RATIOFACTOR;
                    if(aimingflag) // emulate cursor moving back to the center
                        gunx /= emuoverclock ? 1.03f : 1.07f, crosshairx /= emuoverclock ? 1.03f : 1.07f;
                    gunx = ClampFloat(gunx, -GUNAIMLIMIT, GUNAIMLIMIT);
                    crosshairx = ClampFloat(crosshairx, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                    _link->WriteFloat(playerbase[player] + GE_gunx, gunx);
                    _link->WriteFloat(playerbase[player] + GE_crosshairx, crosshairx);
                }
                if((!tankflag && camy > -90 || tankflag && camy > -20) && camy < 90) // only allow player's gun to pitch within a valid range
                {
                    float guny = _link->ReadFloat(playerbase[player] + GE_guny), crosshairy = _link->ReadFloat(playerbase[player] + GE_crosshairy);
                    guny += (!_cfgptr->Profile[player].SETTINGS[INVERTPITCH] ? _cfgptr->Device[player].RSTICKY : -_cfgptr->Device[player].RSTICKY) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_stick_y * (fov / basefov) * 0.025f;
                    guny += (!_cfgptr->Profile[player].SETTINGS[INVERTPITCH] ? _cfgptr->Device[player].GYROY : -_cfgptr->Device[player].GYROY) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_gyro_y * (fov / basefov) * 0.025f;
                    crosshairy += (!_cfgptr->Profile[player].SETTINGS[INVERTPITCH] ? _cfgptr->Device[player].RSTICKY : -_cfgptr->Device[player].RSTICKY) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_stick_y * (fov / 4 / (basefov / 4)) * 0.0225f;
                    crosshairy += (!_cfgptr->Profile[player].SETTINGS[INVERTPITCH] ? _cfgptr->Device[player].RSTICKY : -_cfgptr->Device[player].RSTICKY) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_gyro_y * _cfgptr->Device[player].DELTATIME * (fov / 4 / (basefov / 4)) * 0.0225f;
                    if(aimingflag)
                        guny /= emuoverclock ? 1.15f : 1.35f, crosshairy /= emuoverclock ? 1.15f : 1.35f;
                    guny = ClampFloat(guny, -GUNAIMLIMIT, GUNAIMLIMIT);
                    crosshairy = ClampFloat(crosshairy, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                    _link->WriteFloat(playerbase[player] + GE_guny, guny);
                    _link->WriteFloat(playerbase[player] + GE_crosshairy, crosshairy);
                }
            }
        }
        else if(player == PLAYER1 && menupage != 11 && menupage != 23) // if user is in menu (only player 1 can control menu)
        {
            float menucrosshairx = _link->ReadFloat(GE_menux), menucrosshairy = _link->ReadFloat(GE_menuy);
            menucrosshairx += _cfgptr->Device[player].RSTICKX / 10.0f * sensitivity_stick_x * 6;
            menucrosshairx += _cfgptr->Device[player].GYROX / 10.0f * _cfgptr->Device[player].DELTATIME * sensitivity_gyro_x * 6;
            menucrosshairy += _cfgptr->Device[player].RSTICKY / 10.0f * sensitivity_stick_y * (400.0f / 290.0f * 6); // y is a little weaker then x in the menu so add more power to make it feel even with x axis
            menucrosshairy += _cfgptr->Device[player].GYROY / 10.0f * sensitivity_gyro_y * _cfgptr->Device[player].DELTATIME * (400.0f / 290.0f * 6); // y is a little weaker then x in the menu so add more power to make it feel even with x axis
            menucrosshairx = ClampFloat(menucrosshairx, 20, 420);
            menucrosshairy = ClampFloat(menucrosshairy, 20, 310);
            _link->WriteFloat(GE_menux, menucrosshairx);
            _link->WriteFloat(GE_menuy, menucrosshairy);
        }
        if(dead || menupage != 11) // if player is dead or in menu, reset crouch toggle
            GE_ResetCrouchToggle(player);
    }
    GE_Controller(); // set controller data
    GE_ResetGyro();
}

void Goldeneye::Quit() {
    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        playerbase[player] = 0;
        GE_ResetCrouchToggle(player);
    }
}

Goldeneye::Goldeneye() {

}
