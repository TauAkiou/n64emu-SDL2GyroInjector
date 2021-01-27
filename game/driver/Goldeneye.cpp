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

#include <iostream>
#include "Goldeneye.h"

#define GUNAIMLIMIT 6.879164696 // 0x40DC221E
#define CROSSHAIRLIMIT 5.159373283 // 0x40A51996
#define GUNAIMLIMITFREE 12.879164696 // 0x40DC221E
#define CROSSHAIRLIMITFREE 10.159373283 // 0x40A51996
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

Goldeneye::Goldeneye(EmulatorLink* linkptr) {
    _link = linkptr;
}

int Goldeneye::Status() {
    const int ge_camera = _link->ReadInt(GE_camera), ge_page = _link->ReadInt(GE_menupage), ge_pause = _link->ReadInt(GE_pause), ge_exit = _link->ReadInt(GE_exit);
    const float ge_crosshairx = _link->ReadFloat(GE_menux), ge_crosshairy = _link->ReadFloat(GE_menuy);
    return (ge_camera >= 0 && ge_camera <= 10 && ge_page >= -1 && ge_page <= 25 && ge_pause >= 0 && ge_pause <= 1 && ge_exit >= 0 && ge_exit <= 1 && ge_crosshairx >= 20 && ge_crosshairx <= 420 && ge_crosshairy >= 20 && ge_crosshairy <= 310); // if GoldenEye 007 is current game
}

void Goldeneye::Quit() {
    for(int player = PLAYER1; player < ALLPLAYERS; player++) {
        playerbase[player] = 0;
        GE_ResetCrouchToggle(player);
    }
}

void Goldeneye::Inject() {
    if(_link->ReadInt(GE_menupage) < 1) // hacks can only be injected at boot sequence before code blocks are cached, so inject until the main menu
        _injecthacks();
    const int menupage = _link->ReadInt(GE_menupage);

    for(int player = PLAYER1; player < ALLPLAYERS; player++)
    {
        auto profile = _settings->GetProfileForPlayer(static_cast<enum PLAYERS>(player));
        playerbase[player] = BONDDATA(player);
        const int dead = _link->ReadInt(playerbase[player] + GE_deathflag);


        if(!_settings->GetIfPlayerIsConnected(static_cast<enum PLAYERS>(player))) // bypass disabled players
            continue;

        if(!profile.FreeAiming) {
            _processOriginalAimmode(player, profile);
        }
        else if(profile.FreeAiming) {
            _processFreeAim(player, profile);
        }

        if(player == PLAYER1 && menupage != 11 && menupage != 23) // if user is in menu (only player 1 can control menu)
        {
            _processMenu(player, profile);
        }
        if(dead || menupage != 11) // if player is dead or in menu, reset crouch toggle
            GE_ResetCrouchToggle(player);
    }
    _controller(); // set controller data
    _resetgyro();
}

void Goldeneye::_processOriginalAimmode(int player, const PROFILE& profile) {
    const int camera = _link->ReadInt(GE_camera);
    const int exit = _link->ReadInt(GE_exit);
    const int pause = _link->ReadInt(GE_pause);
    const int menupage = _link->ReadInt(GE_menupage);
    const int tankflag = _link->ReadInt(GE_tankflag);
    const int mproundend = _link->ReadInt(GE_matchended);
    
    vec2<float> aimstickdata = _ihandler.ProcessAimStickInputForPlayer((PLAYERS)player);

    playerbase[player] = BONDDATA(player);
    const int dead = _link->ReadInt(playerbase[player] + GE_deathflag);
    const int watch = _link->ReadInt(playerbase[player] + GE_watch);
    const int aimingflag = _link->ReadInt(playerbase[player] + GE_aimingflag);
    const int mppausemenu = _link->ReadInt(playerbase[player] + GE_multipausemenu);
    const int cursoraimingflag = profile.GoldeneyeAimMode && aimingflag;
    const float fov = _link->ReadFloat(playerbase[player] + GE_fov);
    const float basefov = fov > 60.0f ? (float)OVERRIDEFOV : 60.0f;
    //const float mouseaccel = profile.SETTINGS[ACCELERATION] ? sqrt(_cfgptr->Device[player].XPOS * _cfgptr->Device[player].XPOS + _cfgptr->Device[player].YPOS * _cfgptr->Device[player].YPOS) / TICKRATE / 12.0f * profile.SETTINGS[ACCELERATION] : 0;
    const vec2<float> sensitivity_basefactor_stick = _ihandler.GetBaseFactorForStickType(profile.StickMode);

    const float sensitivity_stick_x = profile.AimStickSensitivity.x * sensitivity_basefactor_stick.x / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_stick_y = profile.AimStickSensitivity.y * sensitivity_basefactor_stick.y / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_gyro_x = profile.GyroscopeSensitivity.x * GYRO_BASEFACTOR / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_gyro_y = profile.GyroscopeSensitivity.y * GYRO_BASEFACTOR / 40.0f; // * fmax(mouseaccel, 1);

    const float gunsensitivity_stick_x = sensitivity_stick_x * (profile.Crosshair / 2.5f);
    const float gunsensitivity_stick_y = sensitivity_stick_y * (profile.Crosshair / 2.5f);
    const float gunsensitivity_gyro_x = sensitivity_gyro_x * (profile.Crosshair / 2.5f);
    const float gunsensitivity_gyro_y = sensitivity_gyro_y * (profile.Crosshair / 2.5f);

    float camx = _link->ReadFloat(playerbase[player] + GE_camx), camy = _link->ReadFloat(playerbase[player] + GE_camy);
    if(camx >= 0 && camx <= 360 && camy >= -90 && camy <= 90 && fov >= 1 && fov <= FOV_MAX && dead == 0 && watch == 0 && pause == 0 && (camera == 4 || camera == 0) && exit == 1 && menupage == 11 && !mproundend && !mppausemenu) // if safe to inject
    {
        _aimmode(player, profile, cursoraimingflag, fov, basefov);
        if(!tankflag) // player is on foot
        {
            _crouch(player, profile); // only allow crouching if player is not in tank
            if(!cursoraimingflag) { // if not aiming (or geaimmode is off)
                camx += aimstickdata.x / 10.0f * sensitivity_stick_x *
                        (fov / basefov); // regular mouselook calculation
                camx += _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x * _cfgptr->DeltaTime * (fov / basefov);
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
                tankx += aimstickdata.x / 10.0f * sensitivity_stick_x / (360 / TANKXROTATIONLIMIT * 2.5) *
                         (fov / basefov);
                tankx += _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x / (360 / TANKXROTATIONLIMIT * 2.5) *
                         _cfgptr->DeltaTime * (fov / basefov);
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
            camy += (!profile.PitchInverted ? -aimstickdata.y : aimstickdata.y) /
                    10.0f * sensitivity_stick_y * (fov / basefov);
            camy += (!profile.PitchInverted ? -_cfgptr->Device[player].GYRO.y : _cfgptr->Device[player].GYRO.y) /
                    10.0f * sensitivity_gyro_y * _cfgptr->DeltaTime * (fov / basefov);
        }
        else
            camy += -aimy[player] * (fov / basefov);
        camy = PluginHelpers::ClampFloat(camy, tankflag ? -20 : -90, 90); // tank limits player from looking down -20
        _link->WriteFloat(playerbase[player] + GE_camy, camy);
        if(profile.Crosshair && !cursoraimingflag) // if crosshair movement is enabled and player isn't aiming (don't calculate weapon movement while the player is in aim mode)
        {
            if(!tankflag)
            {
                float gunx = _link->ReadFloat(playerbase[player] + GE_gunx), crosshairx = _link->ReadFloat(playerbase[player] + GE_crosshairx); // after camera x and y have been calculated and injected, calculate the gun/crosshair movement
                gunx += aimstickdata.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / basefov) * 0.019f;
                gunx += _cfgptr->Device[player].GYRO.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * _cfgptr->DeltaTime * (fov / basefov) * 0.019f;
                crosshairx += aimstickdata.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / 4 / (basefov / 4)) * 0.01912f / RATIOFACTOR;
                crosshairx += _cfgptr->Device[player].GYRO.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * (fov / 4 / (basefov / 4)) * _cfgptr->DeltaTime * 0.01912f / RATIOFACTOR;
                if(aimingflag) // emulate cursor moving back to the center
                    gunx /= _settings->GetIfEmulatorOverclocked() ? 1.03f : 1.07f, crosshairx /= _settings->GetIfEmulatorOverclocked() ? 1.03f : 1.07f;
                gunx = PluginHelpers::ClampFloat(gunx, -GUNAIMLIMIT, GUNAIMLIMIT);
                crosshairx = PluginHelpers::ClampFloat(crosshairx, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                _link->WriteFloat(playerbase[player] + GE_gunx, gunx);
                _link->WriteFloat(playerbase[player] + GE_crosshairx, crosshairx);
            }
            if((!tankflag && camy > -90 || tankflag && camy > -20) && camy < 90) // only allow player's gun to pitch within a valid range
            {
                float guny = _link->ReadFloat(playerbase[player] + GE_guny), crosshairy = _link->ReadFloat(playerbase[player] + GE_crosshairy);
                guny += (!profile.PitchInverted ? aimstickdata.y : -aimstickdata.y) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_stick_y * (fov / basefov) * 0.025f;
                guny += (!profile.PitchInverted ? _cfgptr->Device[player].GYRO.y : -_cfgptr->Device[player].GYRO.y) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_gyro_y * (fov / basefov) * 0.025f;
                crosshairy += (!profile.PitchInverted ? aimstickdata.y : -aimstickdata.y) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_stick_y * (fov / 4 / (basefov / 4)) * 0.0225f;
                crosshairy += (!profile.PitchInverted ? _cfgptr->Device[player].GYRO.x : -_cfgptr->Device[player].GYRO.y) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_gyro_y * _cfgptr->DeltaTime * (fov / 4 / (basefov / 4)) * 0.0225f;
                if(aimingflag)
                    guny /= _settings->GetIfEmulatorOverclocked() ? 1.15f : 1.35f, crosshairy /= _settings->GetIfEmulatorOverclocked() ? 1.15f : 1.35f;
                guny = PluginHelpers::ClampFloat(guny, -GUNAIMLIMIT, GUNAIMLIMIT);
                crosshairy = PluginHelpers::ClampFloat(crosshairy, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                _link->WriteFloat(playerbase[player] + GE_guny, guny);
                _link->WriteFloat(playerbase[player] + GE_crosshairy, crosshairy);
            }
        }
    }
}

void Goldeneye::_aimmode(const int player, const PROFILE& profile, const int aimingflag, const float fov, const float basefov)
{
    const float crosshairx = _link->ReadFloat(playerbase[player] + GE_crosshairx);
    const float crosshairy = _link->ReadFloat(playerbase[player] + GE_crosshairy);
    const float offsetpos[2][33] = {{0, 0, 0, 0, 0.1625, 0.1625, 0.15, 0.5, 0.8, 0.4, 0.5, 0.5, 0.48, 0.9, 0.25, 0.6, 0.6, 0.7, 0.25, 0.15, 0.1625, 0.1625, 0.5, 0.5, 0.9, 0.9, 0, 0, 0, 0, 0, 0.4}, {0, 0, 0, 0, 0.1, 0.1, 0.2, 0.325, 1, 0.3, 0.425, 0.425, 0.45, 0.95, 0.1, 0.55, 0.5, 0.7, 0.25, 0.1, 0.1, 0.1, 0.275, 1, 0.9, 0.8, 0, 0, 0, 0, 0, 0.25}}; // table of X/Y offset for weapons
    const int currentweapon = _link->ReadInt(playerbase[player] + GE_currentweapon);
    const float fovratio = fov / basefov, fovmodifier = basefov / 60.f; // basefov is 60 unless override is above 60
    const float threshold = 0.72f, speed = 475.f, sensitivity = 292.f * fovmodifier;
    const int aimingintank = _link->ReadInt(GE_tankflag) == 1 && currentweapon == 32; // flag if player is driving tank with tank equipped as weapon
    if(aimingflag) // if player is aiming
    {
        //const float mouseaccel = profile.SETTINGS[ACCELERATION] ? sqrt(_cfgptr->Device[player].XPOS * _cfgptr->Device[player].XPOS + _cfgptr->Device[player].YPOS * _cfgptr->Device[player].YPOS) / TICKRATE / 12.0f * profile.SETTINGS[ACCELERATION] : 0;
        if(profile.UseStickToAim) {
            crosshairposx[player] += _cfgptr->Device[player].AIMSTICK.x / 10.0f *
                                     (profile.AimStickSensitivity.x / sensitivity /
                                      RATIOFACTOR); // fmax(mouseaccel, 1); // calculate the crosshair position
            crosshairposy[player] +=
                    (!profile.PitchInverted ? _cfgptr->Device[player].AIMSTICK.y
                                                               : -_cfgptr->Device[player].AIMSTICK.y) / 10.0f *
                    (profile.AimStickSensitivity.y / sensitivity); // fmax(mouseaccel, 1);
        }

        crosshairposx[player] += _cfgptr->Device[player].GYRO.x / 10.0f * (profile.GyroscopeSensitivity.x / sensitivity / RATIOFACTOR) * _cfgptr->DeltaTime; // fmax(mouseaccel, 1);
        crosshairposy[player] += (!profile.PitchInverted ? _cfgptr->Device[player].GYRO.y : -_cfgptr->Device[player].GYRO.y) / 10.0f * (profile.GyroscopeSensitivity.y / sensitivity) * _cfgptr->DeltaTime; // fmax(mouseaccel, 1);

        crosshairposx[player] = PluginHelpers::ClampFloat(crosshairposx[player], -CROSSHAIRLIMIT, CROSSHAIRLIMIT); // apply clamp then inject
        crosshairposy[player] = PluginHelpers::ClampFloat(crosshairposy[player], -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
        if(aimingintank) // if player is aiming while driving tank with tank equipped as weapon, set x axis crosshair to 0 (like the original game - so you cannot aim across the screen because the tank barrel is locked in the center)
            crosshairposx[player] = 0;
        _link->WriteFloat(playerbase[player] + GE_crosshairx, crosshairposx[player]);
        _link->WriteFloat(playerbase[player] + GE_crosshairy, crosshairposy[player]);
        _link->WriteFloat(playerbase[player] + GE_gunx, crosshairposx[player] * RATIOFACTOR * (1.11f + (currentweapon >= 0 && currentweapon <= 32 ? offsetpos[0][currentweapon] : 0.15f) * 1.5f) + fovratio - 1); // calculate and inject the gun angles (uses pre-made pos table or if unknown weapon use fail-safe value)
        _link->WriteFloat(playerbase[player] + GE_guny, crosshairposy[player] * (1.11f + (currentweapon >= 0 && currentweapon <= 32 ? offsetpos[1][currentweapon] : 0) * 1.5f) + fovratio - 1);
        if(crosshairx > 0 && crosshairx / CROSSHAIRLIMIT > threshold) // if crosshair is within threshold of the border then calculate a linear scrolling speed and enable mouselook
            aimx[player] = (crosshairx / CROSSHAIRLIMIT - threshold) * speed * TIMESTEP;
        else if(crosshairx < 0 && crosshairx / CROSSHAIRLIMIT < -threshold)
            aimx[player] = (crosshairx / CROSSHAIRLIMIT + threshold) * speed * TIMESTEP;
        else
            aimx[player] = 0;
        if(crosshairy > 0 && crosshairy / CROSSHAIRLIMIT > threshold)
            aimy[player] = (crosshairy / CROSSHAIRLIMIT - threshold) * speed * TIMESTEP;
        else if(crosshairy < 0 && crosshairy / CROSSHAIRLIMIT < -threshold)
            aimy[player] = (crosshairy / CROSSHAIRLIMIT + threshold) * speed * TIMESTEP;
        else
            aimy[player] = 0;
    }
    else // player is not aiming so reset crosshairposxy values
        crosshairposx[player] = crosshairx, crosshairposy[player] = crosshairy;
}

void Goldeneye::_processFreeAim(int player, const PROFILE& profile) {
    const int camera = _link->ReadInt(GE_camera);
    const int exit = _link->ReadInt(GE_exit);
    const int pause = _link->ReadInt(GE_pause);
    const int menupage = _link->ReadInt(GE_menupage);
    const int tankflag = _link->ReadInt(GE_tankflag);
    const int mproundend = _link->ReadInt(GE_matchended);

    vec2<float> aimstickdata = _ihandler.ProcessAimStickInputForPlayer((PLAYERS)player);

    playerbase[player] = BONDDATA(player);
    const int dead = _link->ReadInt(playerbase[player] + GE_deathflag);
    const int watch = _link->ReadInt(playerbase[player] + GE_watch);
    const int aimingflag = _link->ReadInt(playerbase[player] + GE_aimingflag);
    const int mppausemenu = _link->ReadInt(playerbase[player] + GE_multipausemenu);
    const int cursoraimingflag = profile.GoldeneyeAimMode && aimingflag;
    const float fov = _link->ReadFloat(playerbase[player] + GE_fov);
    const float basefov = fov > 60.0f ? (float)OVERRIDEFOV : 60.0f;
    //const float mouseaccel = profile.SETTINGS[ACCELERATION] ? sqrt(_cfgptr->Device[player].XPOS * _cfgptr->Device[player].XPOS + _cfgptr->Device[player].YPOS * _cfgptr->Device[player].YPOS) / TICKRATE / 12.0f * profile.SETTINGS[ACCELERATION] : 0;
    const vec2<float> sensitivity_basefactor_stick = _ihandler.GetBaseFactorForStickType(profile.StickMode);

    const float sensitivity_stick_x = profile.AimStickSensitivity.x * sensitivity_basefactor_stick.x / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_stick_y = profile.AimStickSensitivity.y * sensitivity_basefactor_stick.y / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_gyro_x = 400 / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_gyro_y = 400 / 40.0f; // * fmax(mouseaccel, 1);

    const float gunsensitivity_stick_x = sensitivity_stick_x * (profile.Crosshair / 2.5f);
    const float gunsensitivity_stick_y = sensitivity_stick_y * (profile.Crosshair / 2.5f);
    const float gunsensitivity_gyro_x = sensitivity_gyro_x * (profile.Crosshair / 2.5f);
    const float gunsensitivity_gyro_y = sensitivity_gyro_y * (profile.Crosshair / 2.5f);

    float camx = _link->ReadFloat(playerbase[player] + GE_camx), camy = _link->ReadFloat(playerbase[player] + GE_camy);
    if(camx >= 0 && camx <= 360 && camy >= -90 && camy <= 90 && fov >= 1 && fov <= FOV_MAX && dead == 0 && watch == 0 && pause == 0 && (camera == 4 || camera == 0) && exit == 1 && menupage == 11 && !mproundend && !mppausemenu) // if safe to inject
    {
        _aimmode_freeaim(player, profile, cursoraimingflag, fov, basefov);
        if(!tankflag) // player is on foot.
        {
            // In free aim mode, gyro will not alter the camera position.
            _crouch(player, profile); // only allow crouching if player is not in tank
            if(!cursoraimingflag) { // if not aiming (or geaimmode is off)
                camx += aimstickdata.x / 10.0f * sensitivity_stick_x *
                        (fov / basefov); // regular mouselook calculation
                //camx += _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x * _cfgptr->DeltaTime * (fov / basefov);
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
                tankx += aimstickdata.x / 10.0f * sensitivity_stick_x / (360 / TANKXROTATIONLIMIT * 2.5) *
                         (fov / basefov);
                //tankx += _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x / (360 / TANKXROTATIONLIMIT * 2.5) *
                         //_cfgptr->DeltaTime * (fov / basefov);
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

            camy += (!profile.PitchInverted ? -aimstickdata.y : aimstickdata.y) /
                    10.0f * sensitivity_stick_y * (fov / basefov);

            float crosshairy = _link->ReadFloat(playerbase[player] + GE_crosshairy);
            if(crosshairy > 5 || crosshairy < -5)
            camy += (!profile.PitchInverted ? -_cfgptr->Device[player].GYRO.y : _cfgptr->Device[player].GYRO.y) /
                    10.0f * sensitivity_gyro_y * _cfgptr->DeltaTime * (fov / basefov);
        }
        else
            camy += -aimy[player] * (fov / basefov);
        camy = PluginHelpers::ClampFloat(camy, tankflag ? -20 : -90, 90); // tank limits player from looking down -20
        _link->WriteFloat(playerbase[player] + GE_camy, camy);
        if(profile.Crosshair && !cursoraimingflag) // if crosshair movement is enabled and player isn't aiming (don't calculate weapon movement while the player is in aim mode)
        {
            if(!tankflag)
            {
                float gunx = _link->ReadFloat(playerbase[player] + GE_gunx), crosshairx = _link->ReadFloat(playerbase[player] + GE_crosshairx); // after camera x and y have been calculated and injected, calculate the gun/crosshair movement
                //gunx += aimstickdata.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / basefov) * 0.019f;
                gunx += _cfgptr->Device[player].GYRO.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * _cfgptr->DeltaTime * (fov / basefov) * 0.019f;
                //crosshairx += aimstickdata.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / 4 / (basefov / 4)) * 0.01912f / RATIOFACTOR;
                crosshairx += _cfgptr->Device[player].GYRO.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * (fov / 4 / (basefov / 4)) * _cfgptr->DeltaTime * 0.01912f / RATIOFACTOR;
                if(aimingflag) // emulate cursor moving back to the center
                   gunx /= _settings->GetIfEmulatorOverclocked() ? 1.03f : 1.07f, crosshairx /= _settings->GetIfEmulatorOverclocked() ? 1.03f : 1.07f;
                gunx = PluginHelpers::ClampFloat(gunx, -GUNAIMLIMITFREE, GUNAIMLIMITFREE);
                crosshairx = PluginHelpers::ClampFloat(crosshairx, -CROSSHAIRLIMITFREE, CROSSHAIRLIMITFREE);
                _link->WriteFloat(playerbase[player] + GE_gunx, gunx);
                _link->WriteFloat(playerbase[player] + GE_crosshairx, crosshairx);
            }
            if((!tankflag && camy > -90 || tankflag && camy > -20) && camy < 90) // only allow player's gun to pitch within a valid range
            {
                float guny = _link->ReadFloat(playerbase[player] + GE_guny), crosshairy = _link->ReadFloat(playerbase[player] + GE_crosshairy);
                guny += (!profile.PitchInverted ? aimstickdata.y : -aimstickdata.y) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_stick_y * (fov / basefov) * 0.025f;
                guny += (!profile.PitchInverted ? _cfgptr->Device[player].GYRO.y : -_cfgptr->Device[player].GYRO.y) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_gyro_y * (fov / basefov) * 0.025f;
                crosshairy += (!profile.PitchInverted ? aimstickdata.y : -aimstickdata.y) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_stick_y * (fov / 4 / (basefov / 4)) * 0.0225f;
                crosshairy += (!profile.PitchInverted ? _cfgptr->Device[player].GYRO.x : -_cfgptr->Device[player].GYRO.y) / (!aimingflag ? 40.0f : 20.0f) * gunsensitivity_gyro_y * _cfgptr->DeltaTime * (fov / 4 / (basefov / 4)) * 0.0225f;

                if(crosshairy > 45 || crosshairy < -45) {
                    camy += (!profile.PitchInverted ? -_cfgptr->Device[player].GYRO.y : _cfgptr->Device[player].GYRO.y) /
                            10.0f * sensitivity_gyro_y * _cfgptr->DeltaTime * (fov / basefov);
                }

                if(aimingflag)
                    guny /= _settings->GetIfEmulatorOverclocked() ? 1.15f : 1.35f, crosshairy /= _settings->GetIfEmulatorOverclocked() ? 1.15f : 1.35f;
                guny = PluginHelpers::ClampFloat(guny, -GUNAIMLIMIT, GUNAIMLIMIT);

                crosshairy = PluginHelpers::ClampFloat(crosshairy, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                _link->WriteFloat(playerbase[player] + GE_guny, guny);
                _link->WriteFloat(playerbase[player] + GE_crosshairy, crosshairy);
            }
        }
    }
}



void Goldeneye::_crouch(const int player, const PROFILE& profile)
{
    int crouchheld = _cfgptr->Device[player].BUTTONPRIM[CROUCH] || _cfgptr->Device[player].BUTTONSEC[CROUCH] || _cfgptr->Device[player].BUTTONPRIM[KNEEL] || _cfgptr->Device[player].BUTTONSEC[KNEEL];
    if(profile.CrouchToggle) // check and toggle player stance
    {
        if(safetocrouch[player] && crouchheld) // standing to crouching
            safetocrouch[player] = 0, crouchstance[player] = 1;
        else if(!safetocrouch[player] && !crouchheld) // crouch is no longer being held, ready to stand
            safetostand[player] = 1;
        if(safetostand[player] && crouchheld) // stand up
            safetocrouch[player] = 1, crouchstance[player] = 0;
        else if(safetostand[player] && safetocrouch[player] && !crouchheld) // crouch key not active, ready to toggle
            safetostand[player] = 0;
        crouchheld = crouchstance[player];
    }
    _link->WriteInt(playerbase[player] + GE_stanceflag, !crouchheld ? 2 : 0); // set in-game stance
}



void Goldeneye::_aimmode_freeaim(const int player, const PROFILE& profile, const int aimingflag, const float fov, const float basefov) {
    const float crosshairx = _link->ReadFloat(playerbase[player] + GE_crosshairx);
    const float crosshairy = _link->ReadFloat(playerbase[player] + GE_crosshairy);
    const float offsetpos[2][33] = {{0, 0, 0, 0, 0.1625, 0.1625, 0.15, 0.5, 0.8, 0.4, 0.5, 0.5, 0.48, 0.9, 0.25, 0.6, 0.6, 0.7, 0.25, 0.15, 0.1625, 0.1625, 0.5, 0.5, 0.9, 0.9, 0, 0, 0, 0, 0, 0.4}, {0, 0, 0, 0, 0.1, 0.1, 0.2, 0.325, 1, 0.3, 0.425, 0.425, 0.45, 0.95, 0.1, 0.55, 0.5, 0.7, 0.25, 0.1, 0.1, 0.1, 0.275, 1, 0.9, 0.8, 0, 0, 0, 0, 0, 0.25}}; // table of X/Y offset for weapons
    const int currentweapon = _link->ReadInt(playerbase[player] + GE_currentweapon);
    const float fovratio = fov / basefov, fovmodifier = basefov / 60.f; // basefov is 60 unless override is above 60
    const float threshold = 0.72f, speed = 475.f, sensitivity = 292.f * fovmodifier;
    const int aimingintank = _link->ReadInt(GE_tankflag) == 1 && currentweapon == 32; // flag if player is driving tank with tank equipped as weapon
    //if(aimingflag) // if player is aiming
    //{
        //const float mouseaccel = profile.SETTINGS[ACCELERATION] ? sqrt(_cfgptr->Device[player].XPOS * _cfgptr->Device[player].XPOS + _cfgptr->Device[player].YPOS * _cfgptr->Device[player].YPOS) / TICKRATE / 12.0f * profile.SETTINGS[ACCELERATION] : 0;
        if(profile.UseStickToAim) {
            crosshairposx[player] += _cfgptr->Device[player].AIMSTICK.x / 10.0f *
                                     ((profile.AimStickSensitivity.x) / sensitivity /
                                      RATIOFACTOR); // fmax(mouseaccel, 1); // calculate the crosshair position
            crosshairposy[player] +=
                    (!profile.PitchInverted ? _cfgptr->Device[player].AIMSTICK.y
                                                               : -_cfgptr->Device[player].AIMSTICK.y) / 10.0f *
                    ((profile.AimStickSensitivity.y) / sensitivity); // fmax(mouseaccel, 1);
        }

        crosshairposx[player] += _cfgptr->Device[player].GYRO.x / 10.0f * ((profile.GyroscopeSensitivity.x * GYRO_BASEFACTOR) / sensitivity / RATIOFACTOR) * _cfgptr->DeltaTime; // fmax(mouseaccel, 1);
        crosshairposy[player] += (!profile.PitchInverted ? _cfgptr->Device[player].GYRO.y : -_cfgptr->Device[player].GYRO.y) / 10.0f * ((profile.GyroscopeSensitivity.y * GYRO_BASEFACTOR) / sensitivity) * _cfgptr->DeltaTime; // fmax(mouseaccel, 1);

        crosshairposx[player] = PluginHelpers::ClampFloat(crosshairposx[player], -CROSSHAIRLIMITFREE, CROSSHAIRLIMITFREE); // apply clamp then inject
        crosshairposy[player] = PluginHelpers::ClampFloat(crosshairposy[player], -CROSSHAIRLIMITFREE, CROSSHAIRLIMITFREE);
        if(aimingintank) // if player is aiming while driving tank with tank equipped as weapon, set x axis crosshair to 0 (like the original game - so you cannot aim across the screen because the tank barrel is locked in the center)
            crosshairposx[player] = 0;
        _link->WriteFloat(playerbase[player] + GE_crosshairx, crosshairposx[player]);
        _link->WriteFloat(playerbase[player] + GE_crosshairy, crosshairposy[player]);
        _link->WriteFloat(playerbase[player] + GE_gunx, crosshairposx[player] * RATIOFACTOR * (1.11f + (currentweapon >= 0 && currentweapon <= 32 ? offsetpos[0][currentweapon] : 0.15f) * 1.5f) + fovratio - 1); // calculate and inject the gun angles (uses pre-made pos table or if unknown weapon use fail-safe value)
        _link->WriteFloat(playerbase[player] + GE_guny, crosshairposy[player] * (1.11f + (currentweapon >= 0 && currentweapon <= 32 ? offsetpos[1][currentweapon] : 0) * 1.5f) + fovratio - 1);
        if(crosshairx > 0 && crosshairx / CROSSHAIRLIMIT > threshold) // if crosshair is within threshold of the border then calculate a linear scrolling speed and enable mouselook
            aimx[player] = (crosshairx / CROSSHAIRLIMIT - threshold) * speed * TIMESTEP;
        else if(crosshairx < 0 && crosshairx / CROSSHAIRLIMIT < -threshold)
            aimx[player] = (crosshairx / CROSSHAIRLIMIT + threshold) * speed * TIMESTEP;
        else
            aimx[player] = 0;
        if(crosshairy > 0 && crosshairy / CROSSHAIRLIMIT > threshold)
            aimy[player] = (crosshairy / CROSSHAIRLIMIT - threshold) * speed * TIMESTEP;
        else if(crosshairy < 0 && crosshairy / CROSSHAIRLIMIT < -threshold)
            aimy[player] = (crosshairy / CROSSHAIRLIMIT + threshold) * speed * TIMESTEP;
        else
            aimy[player] = 0;
    }
    //else // don't reset crosshair positioning in free aim.
        //crosshairposx[player] = crosshairx, crosshairposy[player] = crosshairy;





void Goldeneye::_controller() {
    for(int player = PLAYER1; player < ALLPLAYERS; player++)
    {
        Emulator::Controller[player].U_CBUTTON = _cfgptr->Device[player].BUTTONPRIM[FORWARDS] || _cfgptr->Device[player].BUTTONSEC[FORWARDS];
        Emulator::Controller[player].D_CBUTTON = _cfgptr->Device[player].BUTTONPRIM[BACKWARDS] || _cfgptr->Device[player].BUTTONSEC[BACKWARDS];
        Emulator::Controller[player].L_CBUTTON = _cfgptr->Device[player].BUTTONPRIM[STRAFELEFT] || _cfgptr->Device[player].BUTTONSEC[STRAFELEFT];
        Emulator::Controller[player].R_CBUTTON = _cfgptr->Device[player].BUTTONPRIM[STRAFERIGHT] || _cfgptr->Device[player].BUTTONSEC[STRAFERIGHT];
        Emulator::Controller[player].Z_TRIG = _cfgptr->Device[player].BUTTONPRIM[FIRE] || _cfgptr->Device[player].BUTTONSEC[FIRE] || _cfgptr->Device[player].BUTTONPRIM[PREVIOUSWEAPON] || _cfgptr->Device[player].BUTTONSEC[PREVIOUSWEAPON];
        Emulator::Controller[player].R_TRIG = _cfgptr->Device[player].BUTTONPRIM[AIM] || _cfgptr->Device[player].BUTTONSEC[AIM];
        Emulator::Controller[player].A_BUTTON = _cfgptr->Device[player].BUTTONPRIM[ACCEPT] || _cfgptr->Device[player].BUTTONSEC[ACCEPT] || _cfgptr->Device[player].BUTTONPRIM[PREVIOUSWEAPON] || _cfgptr->Device[player].BUTTONSEC[PREVIOUSWEAPON] || _cfgptr->Device[player].BUTTONPRIM[NEXTWEAPON] || _cfgptr->Device[player].BUTTONSEC[NEXTWEAPON];
        Emulator::Controller[player].B_BUTTON = _cfgptr->Device[player].BUTTONPRIM[CANCEL] || _cfgptr->Device[player].BUTTONSEC[CANCEL];
        Emulator::Controller[player].START_BUTTON = _cfgptr->Device[player].BUTTONPRIM[START] || _cfgptr->Device[player].BUTTONSEC[START];
        _cfgptr->Device[player].ARROW[0] = (_cfgptr->Device[player].BUTTONPRIM[UP] || _cfgptr->Device[player].BUTTONSEC[UP]) ? 127 : 0;
        _cfgptr->Device[player].ARROW[1] = (_cfgptr->Device[player].BUTTONPRIM[DOWN] || _cfgptr->Device[player].BUTTONSEC[DOWN]) ? (_link->ReadInt(GE_menupage) != 11 ? -127 : -128) : 0; // clamp to -127 for menus due to overflow bug
        _cfgptr->Device[player].ARROW[2] = (_cfgptr->Device[player].BUTTONPRIM[LEFT] || _cfgptr->Device[player].BUTTONSEC[LEFT]) ? -128 : 0;
        _cfgptr->Device[player].ARROW[3] = (_cfgptr->Device[player].BUTTONPRIM[RIGHT] || _cfgptr->Device[player].BUTTONSEC[RIGHT]) ? 127 : 0;
        Emulator::Controller[player].X_AXIS = _cfgptr->Device[player].ARROW[0] + _cfgptr->Device[player].ARROW[1];
        Emulator::Controller[player].Y_AXIS = _cfgptr->Device[player].ARROW[2] + _cfgptr->Device[player].ARROW[3];
    }
    if(_link->ReadInt(GE_menupage) != 11 && !Emulator::Controller[PLAYER1].B_BUTTON) // pressing aim will act like the back button (only in menus and for player 1)
        Emulator::Controller[PLAYER1].B_BUTTON = _cfgptr->Device[PLAYER1].BUTTONPRIM[AIM] || _cfgptr->Device[PLAYER1].BUTTONSEC[AIM];
}

//==========================================================================
// Purpose: inject hacks into rom before code has been cached
//==========================================================================
void Goldeneye::_injecthacks()
{
    const unsigned int addressarray[27] = {0x000B7EA0, 0x000B7EB8, 0x0009C7F8, 0x0009C7FC, 0x0009C80C, 0x0009C810, 0x0009C998, 0x0009C99C, 0x0009C9AC, 0x0009C9B0, 0x000AE4DC, 0x000AE4E0, 0x000AE4E4, 0x000AE4E8, 0x000AE4EC, 0x000AE4F0, 0x000AE4F4, 0x000AE4F8, 0x000AE4FC, 0x000AE500, 0x000AE504, 0x000AE508, 0x000AE50C, 0x000AE510, 0x000AE514, 0x000AE518, 0x000AE51C}, codearray[27] = {0x00000000, 0x00000000, 0x0BC1E66B, 0x460C5100, 0x0BC1E66F, 0x460E3280, 0x0BC1E673, 0x460C4100, 0x0BC1E677, 0x460E5200, 0x8C590124, 0x53200001, 0xE4440FF0, 0x0BC19F34, 0x8C590124, 0x53200001, 0xE44A0FF4, 0x0BC19F39, 0x8C590124, 0x53200001, 0xE4441004, 0x0BC19F9C, 0x8C590124, 0x53200001, 0xE4481008, 0x0BC19FA1, 0x00000000}; // disable autostand code, add branch to crosshair code so cursor aiming mode is absolute (without jitter)
    for(int index = 0; index < 27; index++) // inject code array
        _link->WriteROM(addressarray[index], codearray[index]);
#ifndef SPEEDRUN_BUILD // gives unfair advantage, remove for speedrun build
    if((unsigned int)_link->ReadROM(GE_controlstyle) == 0x8DC22A58) // if safe to overwrite
        _link->WriteROM(GE_controlstyle, 0x34020001); // always force game to use 1.2 control style
    if((unsigned int)_link->ReadROM(GE_reversepitch) == 0x8C420A84) // if safe to overwrite
        _link->WriteROM(GE_reversepitch, 0x34020001); // always force game to use upright pitch
    if((unsigned int)_link->ReadInt(GE_pickupyaxisthreshold) == 0xBF490FDB && _link->ReadInt(GE_menupage) == 0) // if safe to overwrite
        _link->WriteFloat(GE_pickupyaxisthreshold, -60.f * PI / 180.f); // overwrite default y axis limit for picking up items (from -45 to -60)
    if(OVERRIDEFOV != 60) // override default fov
    {
        float newfov = OVERRIDEFOV;
        unsigned int unsignedinteger = *(unsigned int *)(float *)(&newfov);
        _link->WriteROM(GE_defaultfov, 0x3C010000 + (short)(unsignedinteger / 0x10000));
        _link->WriteROM(GE_defaultfovinit, 0x3C010000 + (short)(unsignedinteger / 0x10000));
        _link->WriteROM(GE_defaultfovzoom, 0x3C010000 + (short)(unsignedinteger / 0x10000));
        if(_link->ReadInt(GE_weaponypos) == 0 && _link->ReadInt(GE_weaponzpos) == 0) // if first weapon slot position is default
        {
            for(int index = 0; index <= 32; index++) // cycle through first 32 weapons
            {
                const float fovoffset = OVERRIDEFOV - 60;
                const float weaponypos = _link->ReadFloat(GE_weaponypos + (index * 0x70)) - (fovoffset / (2.25f * 4.f)); // adjust weapon Y/Z positions for override field of view
                const float weaponzpos = _link->ReadFloat(GE_weaponzpos + (index * 0x70)) + (fovoffset / 2.75f);
                _link->WriteFloat(GE_weaponypos + (index * 0x70), weaponypos);
                _link->WriteFloat(GE_weaponzpos + (index * 0x70), weaponzpos);
            }
        }
        if(OVERRIDEFOV > 60)
            _link->WriteFloat(GE_defaultzoomspeed, (OVERRIDEFOV - 60) * ((1.7f - 0.909091f) / 60.0f) + 0.909091f); // adjust zoom speed default (0.909091 default, 1.7 max)
    }
    if((unsigned int)_link->ReadInt(GE_defaultratio) == 0x3FE38E39 && (_settings->GetOverrideRatioWidth() != 16 || _settings->GetOverrideRatioHeight() != 9)) // override default 16:9 ratio
        _link->WriteFloat(GE_defaultratio, (float)_settings->GetOverrideRatioWidth() / (float)_settings->GetOverrideRatioHeight());
#endif
    if(_settings->GetShowGoldeneyeCrosshair()) // inject show crosshair hack
    {
        _link->WriteROM(GE_showcrosshair, 0x8C4E01C8); // replace lw $t6, 0x1128 ($v0) (8C4E1128) with lw $t6, 0x01C8 ($v0) (8C4E01C8)
        if(_link->ReadROM(GE_crosshairimage) == 0x000008BC && _link->ReadROM(0x28) != 0x45522020) // if crosshair image found and rom isn't Goldfinger 64 (why? because GF64 replaced beta crosshair image with ammo icon)
            _link->WriteROM(GE_crosshairimage, 0x000008BD); // replace crosshair image with beta crosshair
    }
    if(Emulator::Controller[PLAYER1].Z_TRIG && Emulator::Controller[PLAYER1].R_TRIG) // skip intros if holding down fire + aim
    {
        _link->WriteInt(GE_introcounter, 0x00001000);
        _link->WriteInt(GE_seenintroflag, 0);
    }
}

//==========================================================================
// Purpose: Reset Y coordinate to 0 in the event that a gyro controller needs to be recentered.
//==========================================================================
void Goldeneye::_resetgyro() {
    for(int player_int = 0; player_int < ALLPLAYERS; player_int++) {
        if (_cfgptr->Device[player_int].BUTTONPRIM[RESETGYRO]) {
            auto player = static_cast<PLAYERS>(player_int);
            auto profile = _settings->GetProfileForPlayer(player);
            // Only reset the gun position in Free Aim mode.
            if(profile.FreeAiming) {
                crosshairposx[player] = 0;
                crosshairposy[player] = 0;
                _link->WriteFloat(playerbase[player] + GE_guny, 0);
                _link->WriteFloat(playerbase[player] + GE_crosshairy, 0);
                _link->WriteFloat(playerbase[player] + GE_gunx, 0);
                _link->WriteFloat(playerbase[player] + GE_crosshairx, 0);
                _link->WriteFloat(playerbase[player] + GE_camy, 0);
            }
            else {
                // reset aimy, guny, crosshairy to 0, which represents a flat horizontal field of view.
                _link->WriteFloat(playerbase[player] + GE_guny, 0);
                _link->WriteFloat(playerbase[player] + GE_crosshairy, 0);
                _link->WriteFloat(playerbase[player] + GE_camy, 0);
            }
        }
    }
}


void Goldeneye::_processMenu(int player, const PROFILE& profile) {
    // Use a constant sensitivity to make the menu feel consistent.
    const float sensitivity_stick_menu_x = 100 / 40.0f;
    const float sensitivity_stick_menu_y = 100 / 40.0f;
    const float sensitivity_gyro_menu_x = 600 / 40.0f;
    const float sensitivity_gyro_menu_y = 600 / 40.0f;

    float menucrosshairx = _link->ReadFloat(GE_menux), menucrosshairy = _link->ReadFloat(GE_menuy);
    // Use a 1:1 sensitivity for the menu to provide a simple and consistent experience.
    auto filtered = _ihandler.HandleDeadZoneStickInput(_cfgptr->Device[player].AIMSTICK,
                                                       profile.AimstickDeadzone);
    menucrosshairx += filtered.x / 10.0f * sensitivity_stick_menu_x * 6;
    menucrosshairx += _cfgptr->Device[player].GYRO.x / 10.0f * _cfgptr->DeltaTime * sensitivity_gyro_menu_x * 6;
    menucrosshairy += filtered.y / 10.0f *  sensitivity_stick_menu_y * (400.0f / 290.0f * 6); // y is a little weaker then x in the menu so add more power to make it feel even with x axis
    menucrosshairy += _cfgptr->Device[player].GYRO.y / 10.0f * sensitivity_gyro_menu_y * _cfgptr->DeltaTime * (400.0f / 290.0f * 6); // y is a little weaker then x in the menu so add more power to make it feel even with x axis
    menucrosshairx = PluginHelpers::ClampFloat(menucrosshairx, 20, 420);
    menucrosshairy = PluginHelpers::ClampFloat(menucrosshairy, 20, 310);
    _link->WriteFloat(GE_menux, menucrosshairx);
    _link->WriteFloat(GE_menuy, menucrosshairy);
}


