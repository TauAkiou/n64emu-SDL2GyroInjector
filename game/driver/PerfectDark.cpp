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

#include "PerfectDark.h"

#define GUNAIMLIMIT 14.12940025f // 0x41621206
#define GUNAIMLIMITFREE 22.12940025f
#define CROSSHAIRLIMIT 18.76135635f // 0x41961742
#define CROSSHAIRLIMITFREE 26.76135635f
#define GUNRECOILXLIMIT 756.1247559f // 0x443D07FC
#define GUNRECOILYLIMIT 57.63883972f // 0x42668E2C
#define GUNRECOILXLIMITFREE 756.1247559f // 0x443D07FC
#define GUNRECOILYLIMITFREE 57.63883972f // 0x42668E2C
#define BIKEXROTATIONLIMIT 6.282184601f // 0x40C907A8
#define BIKEROLLLIMIT 0.7852724195f // 0xBF49079D/0x3F49079D
#define PI 3.1415927f // 0x40490FDB
// PERFECT DARK ADDRESSES - OFFSET ADDRESSES BELOW (REQUIRES PLAYERBASE/BIKEBASE TO USE)
#define PD_stanceflag 0x801BB74C - 0x801BB6A0
#define PD_deathflag 0x801BB778 - 0x801BB6A0
#define PD_camx 0x801BB7E4 - 0x801BB6A0
#define PD_camy 0x801BB7F4 - 0x801BB6A0
#define PD_fov 0x801BCEE8 - 0x801BB6A0
#define PD_crosshairx 0x801BCD08 - 0x801BB6A0
#define PD_crosshairy 0x801BCD0C - 0x801BB6A0
#define PD_grabflag 0x801BB850 - 0x801BB6A0
#define PD_bikeptr 0x801BD10C - 0x801BB6A0
#define PD_thirdperson 0x801BB6A0 - 0x801BB6A0
#define PD_gunrx 0x801BC374 - 0x801BB6A0
#define PD_gunry 0x801BC378 - 0x801BB6A0
#define PD_gunlx 0x801BCB18 - 0x801BB6A0
#define PD_gunly 0x801BCB1C - 0x801BB6A0
#define PD_aimingflag 0x801BB7C0 - 0x801BB6A0
#define PD_gunrstate 0x801BC2DC - 0x801BB6A0
#define PD_gunlstate 0x801BCA80 - 0x801BB6A0
#define PD_gunrxrecoil 0x801BBE98 - 0x801BB6A0
#define PD_gunryrecoil 0x801BBE9C - 0x801BB6A0
#define PD_gunlxrecoil 0x801BC63C - 0x801BB6A0
#define PD_gunlyrecoil 0x801BC640 - 0x801BB6A0
#define PD_currentweapon 0x801BCC28 - 0x801BB6A0
#define PD_bikebase 0x805142C4 - 0x805142C0
#define PD_bikeyaw 0x804B1AB4 - 0x804B1A48
#define PD_bikeroll 0x804B1B04 - 0x804B1A48
// Address offsets taken from https://github.com/n64decomp/perfect_dark/blob/master/src/include/types.h
#define PD_speedsideways 0x168
#define PD_speedforwards 0x170
// STATIC ADDRESSES BELOW
#define JOANNADATA(X) (unsigned int)_link->ReadInt(0x8009A024 + (X * 0x4)) // player pointer address (0x4 offset for each players)
#define PD_menu(X) 0x80070750 + (X * 0x4) // player menu flag (0 = PD is in menu) (0x4 offset for each players)
#define PD_camera 0x8009A26C // camera flag (1 = gameplay, 2 & 3 = ???, 4 = multiplayer sweep, 5 = gameover screen, 6 = cutscene mode, 7 = force player to move: extraction's dark room)
#define PD_pause 0x80084014 // menu flag (1 = PD is paused)
#define PD_stageid 0x800624E4 // stage id
#define PD_debugtext 0x803C79E0 // debug text (used to check if PD is running)
#define PD_mppause 0x800ACBA6 // used to check if multiplayer match is paused
#define PD_defaultratio 0x803CD680 // 16:9 ratio default
#define PD_defaultfov 0x802EAA5C // field of view default
#define PD_defaultfovzoom 0x802EACFC // field of view default for zoom
#define PD_defaultzoominspeed 0x802DA8F8 // default zoom in speed
#define PD_defaultzoomoutspeed 0x802DA924 // default zoom out speed
#define PD_introcounter 0x800624C4 // counter for intro
#define PD_controlstyle 0x80372728 // instruction reads the current controller style
#define PD_reversepitch 0x803727A0 // instruction reads the current reverse pitch option
#define PD_pickupyaxisthreshold 0x803CAE78 // y axis threshold on picking up weapons
#define PD_weapontable 0x8006FF1C // weapon pointer table, used to change view model positions
#define PD_radialmenutimer 0x802EA2BC // time instruction for radial menu to appear (15 ticks)
#define PD_radialmenualphainit 0x803D2CDC // initial alpha value for all menus
#define PD_blurfix 0x802DB68C // nop gap on chr function to store our blur fix

int PerfectDark::Status()
{
    const int pd_menu = _link->ReadInt(PD_menu(PLAYER1)), pd_camera = _link->ReadInt(PD_camera), pd_pause = _link->ReadInt(PD_pause), pd_romcheck = _link->ReadInt(PD_debugtext);
    return (pd_menu >= 0 && pd_menu <= 1 && pd_camera >= 0 && pd_camera <= 7 && pd_pause >= 0 && pd_pause <= 1 && pd_romcheck == 0x3E20416C); // if Perfect Dark is current game
}
//==========================================================================
// PerfectDark::Inject()
// Purpose: Get all necessary structs and use settings to pass to correct injection function.
// Changes Members: playerbase
// Changes Globals: safetocrouch, safetostand, crouchstance
//==========================================================================
void PerfectDark::Inject()
{
    if(_link->ReadInt(PD_stageid) < 1) // hacks can only be injected at boot sequence before code blocks are cached, so inject until player has spawned
        _injecthacks();


    const int camera = _link->ReadInt(PD_camera);
    const int pause = _link->ReadInt(PD_pause);
    const int mppause = (_link->ReadShort(PD_mppause) & 0xFF00);

    for(int player_int = PLAYER1; player_int < ALLPLAYERS; player_int++)
    {
        auto player = static_cast<PLAYERS>(player_int);
        if(!_settings->GetIfPlayerIsConnected(player)) // bypass disabled players
            continue;

        js_settings::PROFILE profile = _settings->GetProfileForPlayer(player);


        playerbase[player] = JOANNADATA(player);
        const int dead = _link->ReadInt(playerbase[player] + PD_deathflag);
        const int menu = _link->ReadInt(PD_menu(player));

        if(!profile.FreeAiming) {
            _processOriginalInput(player, profile);
        }
        else if(profile.FreeAiming) {
            _processFreeAimInput(player, profile);
        }

        //_processAnalogMovement(player, profile);
        if(!dead && camera == 1 && !menu) // player is alive, in first person and is in menu
            _radialmenunav(player, profile); // check if player is in radial menu and translate mouse input to C buttons
        else if(dead || camera != 1) // player is dead or not in first person camera
            PD_ResetCrouchToggle(player); // reset crouch toggle
    }
    _controller(); // set controller data
    _resetgyro();
}


//==========================================================================
// PerfectDark::_resetgyro(const int player, const PROFILE& profile)
//
// Purpose: resets camera, crosshair and other positions
// Changes Members (original): None
// Changes Members (Free Aim): crosshairposx, crosshairposy
//==========================================================================
void PerfectDark::_resetgyro() {
    for(int player = 0; player < ALLPLAYERS; player++) {
        auto profile = _settings->GetProfileForPlayer(static_cast<enum PLAYERS>(player));
        if (_cfgptr->Device[player].BUTTONPRIM[RESETGYRO]) {
            // reset aimy, guny, crosshairy to 0, which represents a flat horizontal field of view.
            if(profile.FreeAiming) {
                // In free aim, we need to reset all position values to 0 to 'center' the disconnected crosshair.
                crosshairposx[player] = 0;
                crosshairposy[player] = 0;
                _link->WriteFloat(playerbase[player] + PD_gunry, 0);
                _link->WriteFloat(playerbase[player] + PD_gunryrecoil, 0);
                _link->WriteFloat(playerbase[player] + PD_gunly, 0);
                _link->WriteFloat(playerbase[player] + PD_gunlyrecoil, 0);
                _link->WriteFloat(playerbase[player] + PD_crosshairy, 0);
                _link->WriteFloat(playerbase[player] + PD_gunrx, 0);
                _link->WriteFloat(playerbase[player] + PD_gunrxrecoil, 0);
                _link->WriteFloat(playerbase[player] + PD_gunlx, 0);
                _link->WriteFloat(playerbase[player] + PD_gunlxrecoil, 0);
                _link->WriteFloat(playerbase[player] + PD_crosshairx, 0);
                _link->WriteFloat(playerbase[player] + PD_camy, 0);
            }
            _link->WriteFloat(playerbase[player] + PD_gunry, 0);
            _link->WriteFloat(playerbase[player] + PD_gunryrecoil, 0);
            _link->WriteFloat(playerbase[player] + PD_gunly, 0);
            _link->WriteFloat(playerbase[player] + PD_gunlyrecoil, 0);
            _link->WriteFloat(playerbase[player] + PD_crosshairy, 0);
            _link->WriteFloat(playerbase[player] + PD_camy, 0);
        }
    }
}

//==========================================================================
// PerfectDark::_crouch(const int player, const PROFILE& profile)
//
// Purpose: crouching function for Perfect Dark (2 = stand, 1 = kneel, 0 = duck)
// Changes Members: safetocrouch, crouchstance, safetostand
//==========================================================================
void PerfectDark::_crouch(const int player, const js_settings::PROFILE& profile)
{
    int crouchheld = _cfgptr->Device[player].BUTTONPRIM[CROUCH] || _cfgptr->Device[player].BUTTONSEC[CROUCH];
    int kneelheld = _cfgptr->Device[player].BUTTONPRIM[KNEEL] || _cfgptr->Device[player].BUTTONSEC[KNEEL];
    int stance = 2; // standing by default
    if(crouchheld)
        stance = 0;
    else if(kneelheld)
        stance = 1;
    if(profile.CrouchToggle) // check and toggle player stance
    {
        const int crouchkneelheld = crouchheld || kneelheld; // holding down crouch/kneel
        if(safetocrouch[player] && crouchkneelheld) // stand to crouch/kneel
            safetocrouch[player] = 0, crouchstance[player] = crouchheld ? 0 : 1;
        else if(!safetocrouch[player] && !crouchkneelheld) // crouch/kneel is no longer being held, ready to stand
            safetostand[player] = 1;
        if(safetostand[player] && crouchkneelheld) // toggle to other stance
        {
            if(crouchheld && crouchstance[player] == 0 || kneelheld && crouchstance[player] == 1) // if pressed crouch/kneel twice, stand up
                safetocrouch[player] = 1, crouchstance[player] = 2;
            else
                safetostand[player] = 0, crouchstance[player] = crouchheld ? 0 : 1; // switch to the other crouch stance (kneel/crouch)
        }
        else if(safetostand[player] && safetocrouch[player] && !crouchkneelheld) // crouch/kneel key not active, ready to toggle
            safetostand[player] = 0;
        stance = crouchstance[player];
    }
    _link->WriteInt(playerbase[player] + PD_stanceflag, stance); // set in-game stance
}
//==========================================================================
// PerfectDark::_aimmode(const int player, const PROFILE& profile, const int aimingflag, const float fov, const float basefov)
//
// Purpose: replicate the original aiming system, uses aimx/y to move screen when crosshair is on border of screen
// Changes members: crosshairposx, crosshairposy, gunrcenter, gunlcenter, aimx, aimy
//==========================================================================
void PerfectDark::_aimmode(const int player, const js_settings::PROFILE& profile, const int aimingflag, const float fov, const float basefov)
{
    const float crosshairx = _link->ReadFloat(playerbase[player] + PD_crosshairx), crosshairy = _link->ReadFloat(playerbase[player] + PD_crosshairy);
    const int gunrreload = _link->ReadInt(playerbase[player] + PD_gunrstate) == 1, gunlreload = _link->ReadInt(playerbase[player] + PD_gunlstate) == 1, unarmed = _link->ReadInt(playerbase[player] + PD_currentweapon) < 2;
    const float fovratio = fov / basefov, fovmodifier = basefov / 60.f; // basefov is 60 unless override is above 60
    const float threshold = 0.72f;
    const float speed = 475.f;
    const float sensitivity = 100.f * fovmodifier;
    const float centertime = 60.f;

    vec2<float> aimstickdata = _ihandler.ProcessAimStickInputForPlayer((PLAYERS)player, true);
    const vec2<float> sensitivity_basefactor_stick = _ihandler.GetGeneralBaseFactorForStick();


    if(aimingflag) // if player is aiming
    {
        //const float mouseaccel = profile.SETTINGS[ACCELERATION] ? sqrt(_cfgptr->Device[player].XPOS * _cfgptr->Device[player].XPOS + _cfgptr->Device[player].YPOS * _cfgptr->Device[player].YPOS) / TICKRATE / 12.0f * profile.SETTINGS[ACCELERATION] : 0;
        if(profile.AllowStickInAimMode) {
            crosshairposx[player] += aimstickdata.x / 10.0f * ((profile.AimStickSensitivity.x * sensitivity_basefactor_stick.x / 2) / sensitivity / RATIOFACTOR); // * fmax(mouseaccel, 1); // calculate the crosshair position
            crosshairposy[player] += (!profile.StickPitchInverted ? aimstickdata.y : -aimstickdata.y) / 10.0f * ((profile.AimStickSensitivity.y * sensitivity_basefactor_stick.y / 2) / sensitivity); // * fmax(mouseaccel, 1);
        }
        crosshairposx[player] += _cfgptr->Device[player].GYRO.x / 10.0f * ((profile.GyroscopeSensitivity.x * GYRO_BASEFACTOR) / (sensitivity) / RATIOFACTOR) * _cfgptr->DeltaTime; //* fmax(mouseaccel, 1); // calculate the crosshair position
        crosshairposy[player] += (!profile.GyroPitchInverted ? _cfgptr->Device[player].GYRO.y : -_cfgptr->Device[player].GYRO.y) / 10.0f * ((profile.GyroscopeSensitivity.y * GYRO_BASEFACTOR) / sensitivity) * _cfgptr->DeltaTime; // * fmax(mouseaccel, 1);
        crosshairposx[player] = PluginHelpers::ClampFloat(crosshairposx[player], -CROSSHAIRLIMIT, CROSSHAIRLIMIT); // apply clamp then inject
        crosshairposy[player] = PluginHelpers::ClampFloat(crosshairposy[player], -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
        _link->WriteFloat(playerbase[player] + PD_crosshairx, crosshairposx[player]);
        _link->WriteFloat(playerbase[player] + PD_crosshairy, crosshairposy[player]);
        if(unarmed || gunrreload) // if unarmed or reloading right weapon, remove from gunrcenter
            gunrcenter[player] -= _settings->GetIfEmulatorOverclocked() ? 1 : 2;
        else if(gunrcenter[player] < (int)centertime) // increase gunrcenter over time until it equals centertime
            gunrcenter[player] += _settings->GetIfEmulatorOverclocked() ? 1 : 2;
        if(gunlreload) // if reloading left weapon, remove from gunlcenter
            gunlcenter[player] -= _settings->GetIfEmulatorOverclocked() ? 1 : 2;
        else if(gunlcenter[player] < (int)centertime)
            gunlcenter[player] += _settings->GetIfEmulatorOverclocked() ? 1 : 2;
        if(gunrcenter[player] < 0)
            gunrcenter[player] = 0;
        if(gunlcenter[player] < 0)
            gunlcenter[player] = 0;
        _link->WriteFloat(playerbase[player] + PD_gunrx, (gunrcenter[player] / centertime) * (crosshairposx[player] * 0.75f) + fovratio - 1); // calculate and inject the gun angles
        _link->WriteFloat(playerbase[player] + PD_gunrxrecoil, crosshairposx[player] * (GUNRECOILXLIMIT / CROSSHAIRLIMIT) * fovmodifier * RATIOFACTOR); // set the recoil to the correct rotation (if we don't, then the recoil is always z axis aligned)
        _link->WriteFloat(playerbase[player] + PD_gunry, (gunrcenter[player] / centertime) * (crosshairposy[player] * 0.66f) + fovratio - 1);
        _link->WriteFloat(playerbase[player] + PD_gunryrecoil, crosshairposy[player] * (GUNRECOILYLIMIT / CROSSHAIRLIMIT) * fovmodifier);
        _link->WriteFloat(playerbase[player] + PD_gunlx, (gunlcenter[player] / centertime) * (crosshairposx[player] * 0.75f) + fovratio - 1);
        _link->WriteFloat(playerbase[player] + PD_gunlxrecoil, crosshairposx[player] * (GUNRECOILXLIMIT / CROSSHAIRLIMIT) * fovmodifier * RATIOFACTOR);
        _link->WriteFloat(playerbase[player] + PD_gunly, (gunlcenter[player] / centertime) * (crosshairposy[player] * 0.66f) + fovratio - 1);
        _link->WriteFloat(playerbase[player] + PD_gunlyrecoil, crosshairposy[player] * (GUNRECOILYLIMIT / CROSSHAIRLIMIT) * fovmodifier);
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
    else // player is not aiming so reset crosshairposxy and gunrlcenter
        crosshairposx[player] = crosshairx, crosshairposy[player] = crosshairy, gunrcenter[player] = 0, gunlcenter[player] = 0;
}
//==========================================================================
// PerfectDark::_camspyslayer
//
// Purpose: translate mouse to analog stick for Camspy/Slayer
// Changes Members: xstick, ystick, usingstick
//==========================================================================
void PerfectDark::_camspyslayer(const int player, const js_settings::PROFILE &profile, const int camspyflag, const float sensitivityx, const float sensitivityy)
{
    if(camspyflag)
    {
        if(!_cfgptr->Device[player].BUTTONPRIM[AIM] && !_cfgptr->Device[player].BUTTONSEC[AIM]) // camspy
        {
            xstick[player] = (int)((!profile.StickPitchInverted ? -_cfgptr->Device[player].AIMSTICK.y : _cfgptr->Device[player].AIMSTICK.y) * sensitivityx *  8.0f);
            ystick[player] = (int) (_cfgptr->Device[player].AIMSTICK.x * sensitivityy * 16.0f);
        }
        else // camspy (aiming mode)
        {
            xstick[player] = (int)((!profile.StickPitchInverted ? _cfgptr->Device[player].AIMSTICK.y : -_cfgptr->Device[player].AIMSTICK.y) * sensitivityx * 15.0f);
            ystick[player] = (int)(_cfgptr->Device[player].AIMSTICK.x * sensitivityy * 15.0f);
        }
    }
    else // slayer
    {
        xstick[player] = (int)((!profile.StickPitchInverted ? _cfgptr->Device[player].AIMSTICK.y : -_cfgptr->Device[player].AIMSTICK.y) * sensitivityx * 17.0f);
        ystick[player] = (int)(_cfgptr->Device[player].AIMSTICK.x * sensitivityy * 17.0f);
    }
    xstick[player] = PluginHelpers::ClampInt(xstick[player], -128, 127);
    ystick[player] = PluginHelpers::ClampInt(ystick[player], -128, 127);
    usingstick[player] = 1;
}
//==========================================================================
// PerfectDark::_radialmenunav()
//
// Purpose: translate mouse to weapon radial menu
// Changed Members: xmenu, ymenu, radialmenudirection
//==========================================================================
void PerfectDark::_radialmenunav(const int player, const js_settings::PROFILE& profile)
{
    const float max = 19, threshold = 13, diagonalthres = 0.75;
    if((_cfgptr->Device[player].BUTTONPRIM[ACCEPT] || _cfgptr->Device[player].BUTTONSEC[ACCEPT]) && !_cfgptr->Device[player].BUTTONPRIM[FIRE] && !_cfgptr->Device[player].BUTTONSEC[FIRE]) // if a button is held (reject if fire is pressed so aimx/y can be reset back to center)
    {
        // Don't process gyro for now (option?)
        if (profile.StickMode == FLICK) {
            // Flick requires an absurdly high sensitivity to work, so we need to ensure that the camspy and radial menus have sane sensitivities.
            xmenu[player] +=
                    _cfgptr->Device[player].AIMSTICK.x / 10.0f * 1000 / 40.0f;
            ymenu[player] +=
                    _cfgptr->Device[player].AIMSTICK.y / 10.0f * 1000 / 40.0f;
            xmenu[player] = PluginHelpers::ClampFloat(xmenu[player], -max, max);
            ymenu[player] = PluginHelpers::ClampFloat(ymenu[player], -max, max);
        }
        else {
        xmenu[player] +=
                _cfgptr->Device[player].AIMSTICK.x / 10.0f * profile.AimStickSensitivity.x / 40.0f;
        ymenu[player] +=
                _cfgptr->Device[player].AIMSTICK.y / 10.0f * profile.AimStickSensitivity.y / 40.0f;
        xmenu[player] = PluginHelpers::ClampFloat(xmenu[player], -max, max);
        ymenu[player] = PluginHelpers::ClampFloat(ymenu[player], -max, max);
    }
        if(ymenu[player] < -threshold) // c-up
            radialmenudirection[player][FORWARDS] = 1;
        else if(ymenu[player] > threshold) // c-down
            radialmenudirection[player][BACKWARDS] = 1;
        if(xmenu[player] < -threshold) // c-left
            radialmenudirection[player][STRAFELEFT] = 1;
        else if(xmenu[player] > threshold) // c-right
            radialmenudirection[player][STRAFERIGHT] = 1;
        if(xmenu[player] < -threshold && ymenu[player] < -threshold && (-xmenu[player] + -ymenu[player]) / (max * 2) > diagonalthres) // c-up-left
            radialmenudirection[player][FORWARDS] = 1, radialmenudirection[player][STRAFELEFT] = 1;
        else if(xmenu[player] > threshold && ymenu[player] < -threshold && (xmenu[player] + -ymenu[player]) / (max * 2) > diagonalthres) // c-up-right
            radialmenudirection[player][FORWARDS] = 1, radialmenudirection[player][STRAFERIGHT] = 1;
        else if(xmenu[player] < -threshold && ymenu[player] > threshold && (-xmenu[player] + ymenu[player]) / (max * 2) > diagonalthres) // c-down-left
            radialmenudirection[player][BACKWARDS] = 1, radialmenudirection[player][STRAFELEFT] = 1;
        else if(xmenu[player] > threshold && ymenu[player] > threshold && (xmenu[player] + ymenu[player]) / (max * 2) > diagonalthres) // c-down-right
            radialmenudirection[player][BACKWARDS] = 1, radialmenudirection[player][STRAFERIGHT] = 1;
    }
    else
        xmenu[player] = 0, ymenu[player] = 0;
}
//==========================================================================
// PerfectDark::_controller()
//
// Purpose: calculate and send emulator key combo
// Changes Globals: xstick, ystick, usingstick, radialmenudirection
//==========================================================================
void PerfectDark::_controller()
{
    for(int player = PLAYER1; player < ALLPLAYERS; player++)
    {
        Emulator::Controller[player].U_CBUTTON = _cfgptr->Device[player].BUTTONPRIM[FORWARDS] || _cfgptr->Device[player].BUTTONSEC[FORWARDS] || radialmenudirection[player][FORWARDS];
        Emulator::Controller[player].D_CBUTTON = _cfgptr->Device[player].BUTTONPRIM[BACKWARDS] || _cfgptr->Device[player].BUTTONSEC[BACKWARDS] || radialmenudirection[player][BACKWARDS];
        Emulator::Controller[player].L_CBUTTON = _cfgptr->Device[player].BUTTONPRIM[STRAFELEFT] || _cfgptr->Device[player].BUTTONSEC[STRAFELEFT] || radialmenudirection[player][STRAFELEFT];
        Emulator::Controller[player].R_CBUTTON = _cfgptr->Device[player].BUTTONPRIM[STRAFERIGHT] || _cfgptr->Device[player].BUTTONSEC[STRAFERIGHT] || radialmenudirection[player][STRAFERIGHT];
        Emulator::Controller[player].Z_TRIG = _cfgptr->Device[player].BUTTONPRIM[FIRE] || _cfgptr->Device[player].BUTTONSEC[FIRE] || _cfgptr->Device[player].BUTTONPRIM[PREVIOUSWEAPON] || _cfgptr->Device[player].BUTTONSEC[PREVIOUSWEAPON];
        Emulator::Controller[player].R_TRIG = _cfgptr->Device[player].BUTTONPRIM[AIM] || _cfgptr->Device[player].BUTTONSEC[AIM];
        Emulator::Controller[player].A_BUTTON = _cfgptr->Device[player].BUTTONPRIM[ACCEPT] || _cfgptr->Device[player].BUTTONSEC[ACCEPT] || _cfgptr->Device[player].BUTTONPRIM[PREVIOUSWEAPON] || _cfgptr->Device[player].BUTTONSEC[PREVIOUSWEAPON] || _cfgptr->Device[player].BUTTONPRIM[NEXTWEAPON] || _cfgptr->Device[player].BUTTONSEC[NEXTWEAPON];
        Emulator::Controller[player].B_BUTTON = _cfgptr->Device[player].BUTTONPRIM[CANCEL] || _cfgptr->Device[player].BUTTONSEC[CANCEL];
        Emulator::Controller[player].START_BUTTON = _cfgptr->Device[player].BUTTONPRIM[START] || _cfgptr->Device[player].BUTTONSEC[START];
        if(!usingstick[player]) // player is not using the camspy/slayer
        {
            _cfgptr->Device[player].ARROW[0] = (_cfgptr->Device[player].BUTTONPRIM[UP] || _cfgptr->Device[player].BUTTONSEC[UP]) ? 127 : 0;
            _cfgptr->Device[player].ARROW[1] = (_cfgptr->Device[player].BUTTONPRIM[DOWN] || _cfgptr->Device[player].BUTTONSEC[DOWN]) ? -128 : 0;
            _cfgptr->Device[player].ARROW[2] = (_cfgptr->Device[player].BUTTONPRIM[LEFT] || _cfgptr->Device[player].BUTTONSEC[LEFT]) ? -128 : 0;
            _cfgptr->Device[player].ARROW[3] = (_cfgptr->Device[player].BUTTONPRIM[RIGHT] || _cfgptr->Device[player].BUTTONSEC[RIGHT]) ? 127 : 0;
            Emulator::Controller[player].X_AXIS = _cfgptr->Device[player].ARROW[0] + _cfgptr->Device[player].ARROW[1];
            Emulator::Controller[player].Y_AXIS = _cfgptr->Device[player].ARROW[2] + _cfgptr->Device[player].ARROW[3];
        }
        else // player is using camspy/slayer
        {
            Emulator::Controller[player].X_AXIS = xstick[player];
            Emulator::Controller[player].Y_AXIS = ystick[player];
        }
        PD_ResetXYStick(player); // reset x/y stick
        PD_ResetRadialMenuBtns(player); // reset radialmenudirection
    }
}
//==========================================================================
// PerfectDark::_injecthacks()
//
// Purpose: inject hacks into rom before code has been cached
//==========================================================================
void PerfectDark::_injecthacks()
{
    auto override_ratio = _settings->GetOverrideRatio();
    const unsigned int addressarray[33] = {0x802C07B8, 0x802C07BC, 0x802C07EC, 0x802C07F0, 0x802C07FC, 0x802C0800, 0x802C0808, 0x802C0820, 0x802C0824, 0x802C082C, 0x802C0830, 0x803C7988, 0x803C798C, 0x803C7990, 0x803C7994, 0x803C7998, 0x803C799C, 0x803C79A0, 0x803C79A4, 0x803C79A8, 0x803C79AC, 0x803C79B0, 0x803C79B4, 0x803C79B8, 0x803C79BC, 0x803C79C0, 0x803C79C4, 0x803C79C8, 0x803C79CC, 0x803C79D0, 0x803C79D4, 0x803C79D8, 0x803C79DC}, codearray[33] = {0x0BC69E62, 0x8EA10120, 0x0BC69E67, 0x263107A4, 0x0BC69E6B, 0x4614C500, 0x46120682, 0x0BC69E6F, 0x26100004, 0x0BC69E73, 0x4614C500, 0x54200003, 0x00000000, 0xE6B21668, 0xE6A8166C, 0x0BC281F0, 0x8EA10120, 0x50200001, 0xE6380530, 0x0BC281FD, 0x8EA10120, 0x50200001, 0xE6340534, 0x0BC28201, 0x8EA10120, 0x50200001, 0xE6380530, 0x0BC2820A, 0x8EA10120, 0x50200001, 0xE6340534, 0x0BC2820D, 0x00000000}; // add branch to crosshair code so cursor aiming mode is absolute (without jitter)
    for(int index = 0; index < 33; index++) // inject code array
        _link->WriteInt(addressarray[index], codearray[index]);
#ifndef SPEEDRUN_BUILD // gives unfair advantage, remove for speedrun build
    if((unsigned int)_link->ReadInt(PD_controlstyle) == 0x9042C7FC) // if safe to overwrite
        _link->WriteInt(PD_controlstyle, 0x34020001); // always force game to use 1.2 control style
    if((unsigned int)_link->ReadInt(PD_reversepitch) == 0x000F102B) // if safe to overwrite
        _link->WriteInt(PD_reversepitch, 0x34020001); // always force game to use upright pitch
    if((unsigned int)_link->ReadInt(PD_pickupyaxisthreshold) == 0xBF4907A9) // if safe to overwrite
        _link->WriteFloat(PD_pickupyaxisthreshold, -60.f * PI / 180.f); // overwrite default y axis limit for picking up items (from -45 to -60)
    if((unsigned int)_link->ReadInt(PD_radialmenutimer) == 0x28410010 && _settings->GetIfEmulatorOverclocked()) // make radial menu trigger quicker (from 15 to 8 ticks)
        _link->WriteInt(PD_radialmenutimer, 0x28410009);
    if((unsigned int)_link->ReadInt(PD_radialmenualphainit) == 0x3E99999A) // make radial menus initialize with 75% alpha
        _link->WriteFloat(PD_radialmenualphainit, 0.75f);
    if((unsigned int)_link->ReadInt(PD_blurfix) == 0x00000000) // add code to clear blur value on death
        _link->WriteInt(PD_blurfix, 0xA46002D8); // replace nop with sh $r0, 0x02D8 ($v1)
    if(OVERRIDEFOV != 60) // override default fov
    {
        float newfov = OVERRIDEFOV;
        unsigned int unsignedinteger = *(unsigned int *)(float *)(&newfov);
        _link->WriteInt(PD_defaultfov, 0x3C010000 + (short)(unsignedinteger / 0x10000));
        _link->WriteInt(PD_defaultfovzoom, 0x3C010000 + (short)(unsignedinteger / 0x10000));
        if((unsigned int)_link->ReadInt(_link->ReadInt(PD_weapontable) + 0x30) == 0xC2240000) // if first weapon slot position is default
        {
            for(int index = 0; index < 64; index++) // cycle through first 64 weapons
            {
                const unsigned int weaponptr = _link->ReadInt(PD_weapontable + (index * 4)); // get pointer for weapon slot
                const float fovoffset = OVERRIDEFOV - 60;
                const float weaponypos = _link->ReadFloat(weaponptr + 0x30) - (fovoffset / (2.75f * 4.f)); // adjust weapon Y/Z positions for override field of view
                const float weaponzpos = _link->ReadFloat(weaponptr + 0x34) + (fovoffset / 3.f);
                _link->WriteFloat(weaponptr + 0x30, weaponypos);
                _link->WriteFloat(weaponptr + 0x34, weaponzpos);
            }
        }
        if(OVERRIDEFOV > 60)
        {
            newfov = 15.f / (OVERRIDEFOV / 60.f);
            unsignedinteger = *(unsigned int *)(float *)(&newfov);
            _link->WriteInt(PD_defaultzoominspeed, 0x3C010000 + (short)(unsignedinteger / 0x10000)); // adjust zoom in speed default (15.f)
            newfov = 30.f * (OVERRIDEFOV / 60.f);
            unsignedinteger = *(unsigned int *)(float *)(&newfov);
            _link->WriteInt(PD_defaultzoomoutspeed, 0x3C010000 + (short)(unsignedinteger / 0x10000)); // adjust zoom out speed default (30.f)
        }
    }
    if((unsigned int)_link->ReadInt(PD_defaultratio) == 0x3FAAAAAB && ((float)override_ratio.x != 16 || (float)override_ratio.y != 9)) // override default 16:9 ratio
        _link->WriteFloat(PD_defaultratio, ((float)override_ratio.x / (float)override_ratio.y) / (4.f / 3.f));
#endif
    if(Emulator::Controller[PLAYER1].Z_TRIG && Emulator::Controller[PLAYER1].R_TRIG) // skip intros if holding down fire + aim
        _link->WriteInt(PD_introcounter, 0x00001000);
}
//==========================================================================
// Purpose: run when emulator closes rom
// Changes Globals: playerbase, safetocrouch, safetostand, crouchstance, xmenu, ymenu
//==========================================================================
void PerfectDark::Quit()
{
    for(int player = PLAYER1; player < ALLPLAYERS; player++)
    {
        playerbase[player] = 0;
        PD_ResetCrouchToggle(player);
        xmenu[player] = 0, ymenu[player] = 0;
    }
}

PerfectDark::PerfectDark(EmulatorLink *linkptr) {
    _link = linkptr;
}

//==========================================================================
// PerfectDark::_processOriginalInput(int player, const PROFILE& profile)
//
// Purpose: Inject mouse input for Original Input Scheme.
// Changes Members: safetocrouch, safetostand, crouchstance
//==========================================================================
void PerfectDark::_processOriginalInput(int player, const js_settings::PROFILE& profile) {
    const int camera = _link->ReadInt(PD_camera);
    const int pause = _link->ReadInt(PD_pause);
    const int mppause = (_link->ReadShort(PD_mppause) & 0xFF00);

        vec2<float> aimstickdata = _ihandler.ProcessAimStickInputForPlayer((PLAYERS)player, false);
        vec2<float> aimstickdata_camspy = _ihandler.ProcessAimStickInputForPlayer((PLAYERS)player, true);
        playerbase[player] = JOANNADATA(player);
        const int dead = _link->ReadInt(playerbase[player] + PD_deathflag);
        const int menu = _link->ReadInt(PD_menu(player));
        const int aimingflag = _link->ReadInt(playerbase[player] + PD_aimingflag);
        const int grabflag = _link->ReadInt(playerbase[player] + PD_grabflag);
        const unsigned int bikebase = _link->ReadInt((unsigned int)_link->ReadInt(playerbase[player] + PD_bikeptr) + PD_bikebase);
        const int thirdperson = _link->ReadInt(playerbase[player] + PD_thirdperson);
        const int cursoraimingflag = profile.PerfectDarkAimMode && aimingflag && _link->ReadInt(playerbase[player] + PD_currentweapon) != 50; // don't use cursoraiming when using the horizon scanner
        const float fov = _link->ReadFloat(playerbase[player] + PD_fov);
        const float basefov = fov > 60.0f ? (float)OVERRIDEFOV : 60.0f;
        // const float mouseaccel = profile.SETTINGS[ACCELERATION] ? sqrt(_cfgptr->Device[player].XPOS * _cfgptr->Device[player].XPOS + _cfgptr->Device[player].YPOS * _cfgptr->Device[player].YPOS) / TICKRATE / 12.0f * profile.SETTINGS[ACCELERATION] : 0;
        const vec2<float> sensitivity_basefactor_stick = (thirdperson == 1 || thirdperson == 2) ? InputHandler::GetGeneralBaseFactorForStick() : InputHandler::GetBaseFactorForStickType(profile.StickMode);

        const float sensitivity_stick_x = profile.AimStickSensitivity.x * sensitivity_basefactor_stick.x / 40.0f; // * fmax(mouseaccel, 1);
        const float sensitivity_stick_y = profile.AimStickSensitivity.y * sensitivity_basefactor_stick.y / 40.0f; // * fmax(mouseaccel, 1);
        const float sensitivity_gyro_x = (profile.GyroscopeSensitivity.x * GYRO_BASEFACTOR) / 40.0f; // * fmax(mouseaccel, 1);
        const float sensitivity_gyro_y = (profile.GyroscopeSensitivity.y * GYRO_BASEFACTOR) / 40.0f; // * fmax(mouseaccel, 1);

        const float gunsensitivity_stick_x = sensitivity_stick_x * (profile.Crosshair / 2.5f);
        const float gunsensitivity_stick_y = sensitivity_stick_y * (profile.Crosshair / 2.5f);
        const float gunsensitivity_gyro_x = sensitivity_gyro_x * (profile.Crosshair / 2.5f);
        const float gunsensitivity_gyro_y = sensitivity_gyro_y * (profile.Crosshair / 2.5f);
        float camx = _link->ReadFloat(playerbase[player] + PD_camx), camy = _link->ReadFloat(playerbase[player] + PD_camy);
        if(camx >= 0 && camx <= 360 && camy >= -90 && camy <= 90 && fov >= 1 && fov <= FOV_MAX && dead == 0 && menu == 1 && pause == 0 && mppause == 0 && camera == 1 && (grabflag == 0 || grabflag == 4 || grabflag == 3)) // if safe to inject
        {
            if(thirdperson == 1 || thirdperson == 2) // if player is using the slayer/camspy, translate mouse input to analog stick and continue to next player
            {
                if(profile.StickMode != FLICK) {
                    _camspyslayer(player, profile, thirdperson == 2, sensitivity_stick_x , sensitivity_stick_y);
                }
                else {
                    _camspyslayer(player, profile, thirdperson == 2, 400 / 40.0f, 400 / 40.0f);
                }
                return;
            }
            _aimmode(player, profile, cursoraimingflag, fov, basefov);
            if(grabflag != 3) // if player is on foot
            {
                _crouch(player, profile);
                if(!cursoraimingflag) { // if not aiming (or pdaimmode is off)
                    camx += aimstickdata.x / 10.0f * sensitivity_stick_x *
                            (fov / basefov); // regular mouselook calculation
                    camx += _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x * _cfgptr->DeltaTime *
                            (fov / basefov);
                }
                else
                    camx += aimx[player] * (fov / basefov); // scroll screen with aimx/aimy
                while(camx < 0)
                    camx += 360;
                while(camx >= 360)
                    camx -= 360;
                _link->WriteFloat(playerbase[player] + PD_camx, camx);
            }
            else if(_link->IsWithinRange(bikebase)) // if player is riding hoverbike (and hoverbike address is legal)
            {
                PD_ResetCrouchToggle(player);
                float bikeyaw = _link->ReadFloat(bikebase + PD_bikeyaw), bikeroll = _link->ReadFloat(bikebase + PD_bikeroll);
                if(!cursoraimingflag)
                {
                    bikeyaw -= aimstickdata.x / 10.0f * sensitivity_stick_x / (360 / BIKEXROTATIONLIMIT) * (fov / basefov);
                    bikeyaw -= _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x / (360 / BIKEXROTATIONLIMIT) * _cfgptr->DeltaTime * (fov / basefov);
                    bikeroll += aimstickdata.x / 10.0f * sensitivity_stick_x * (fov / basefov) / 100;
                    bikeroll += _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x * (fov / basefov) * _cfgptr->DeltaTime / 100;
                }
                else
                {
                    bikeyaw -= aimx[player] / (360 / BIKEXROTATIONLIMIT) * (fov / basefov);
                    bikeroll += aimx[player] * sensitivity_stick_x * (fov / basefov) / 100; // Use stick sensitivity (???)
                }
                while(bikeyaw < 0)
                    bikeyaw += BIKEXROTATIONLIMIT;
                while(bikeyaw >= BIKEXROTATIONLIMIT)
                    bikeyaw -= BIKEXROTATIONLIMIT;
                bikeroll = PluginHelpers::ClampFloat(bikeroll, -BIKEROLLLIMIT, BIKEROLLLIMIT);
                _link->WriteFloat(bikebase + PD_bikeyaw, bikeyaw);
                _link->WriteFloat(bikebase + PD_bikeroll, bikeroll);
            }
            if(!cursoraimingflag) {
                camy += (!profile.StickPitchInverted ? -aimstickdata.y : aimstickdata.y) /
                        10.0f * sensitivity_stick_x * (fov / basefov);
                camy += (!profile.GyroPitchInverted ? -_cfgptr->Device[player].GYRO.y : _cfgptr->Device[player].GYRO.y) /
                        10.0f * sensitivity_gyro_x * _cfgptr->DeltaTime * (fov / basefov);
            }
            else {
                camy += -aimy[player] * (fov / basefov);
            }
            camy = PluginHelpers::ClampFloat(camy, -90, 90);
            _link->WriteFloat(playerbase[player] + PD_camy, camy);
            if(profile.Crosshair && !cursoraimingflag) // if crosshair movement is enabled and player isn't aiming (don't calculate weapon movement while the player is in aim mode)
            {
                float gunx = _link->ReadFloat(playerbase[player] + PD_gunrx), crosshairx = _link->ReadFloat(playerbase[player] + PD_crosshairx); // after camera x and y have been calculated and injected, calculate the gun/reload/crosshair movement
                gunx += aimstickdata.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / basefov) * 0.05f / RATIOFACTOR;
                gunx += _cfgptr->Device[player].GYRO.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * _cfgptr->DeltaTime * (fov / basefov) * 0.05f / RATIOFACTOR;
                crosshairx += aimstickdata.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / 4 / (basefov / 4)) * 0.05f / RATIOFACTOR;
                crosshairx += _cfgptr->Device[player].GYRO.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x * _cfgptr->DeltaTime * (fov / 4 / (basefov / 4)) * 0.05f / RATIOFACTOR;
                if(aimingflag) // emulate cursor moving back to the center
                    gunx /= _settings->GetIfEmulatorOverclocked() ? 1.03f : 1.07f, crosshairx /= _settings->GetIfEmulatorOverclocked() ? 1.03f : 1.07f;
                gunx = PluginHelpers::ClampFloat(gunx, -GUNAIMLIMIT, GUNAIMLIMIT);
                crosshairx = PluginHelpers::ClampFloat(crosshairx, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                _link->WriteFloat(playerbase[player] + PD_gunrx, gunx);
                _link->WriteFloat(playerbase[player] + PD_gunrxrecoil, crosshairx * (GUNRECOILXLIMIT / CROSSHAIRLIMIT));
                _link->WriteFloat(playerbase[player] + PD_gunlx, gunx);
                _link->WriteFloat(playerbase[player] + PD_gunlxrecoil, crosshairx * (GUNRECOILXLIMIT / CROSSHAIRLIMIT));
                _link->WriteFloat(playerbase[player] + PD_crosshairx, crosshairx);
                if(camy > -90 && camy < 90) // only allow player's gun to pitch within a valid range
                {
                    float guny = _link->ReadFloat(playerbase[player] + PD_gunry), crosshairy = _link->ReadFloat(playerbase[player] + PD_crosshairy);
                    guny += (!profile.StickPitchInverted ? aimstickdata.y : -aimstickdata.y) / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_y * (fov / basefov) * 0.075f;
                    guny += (!profile.GyroPitchInverted ? _cfgptr->Device[player].GYRO.y : -_cfgptr->Device[player].GYRO.y) / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_y * _cfgptr->DeltaTime * (fov / basefov) * 0.075f;

                    crosshairy += (!profile.StickPitchInverted ? aimstickdata.y : -aimstickdata.x) / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_y * (fov / 4 / (basefov / 4)) * 0.1f;
                    crosshairy += (!profile.GyroPitchInverted ? _cfgptr->Device[player].GYRO.x : -_cfgptr->Device[player].GYRO.x) / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_y * _cfgptr->DeltaTime * (fov / 4 / (basefov / 4)) * 0.1f;
                    if(aimingflag)
                        guny /= _settings->GetIfEmulatorOverclocked() ? 1.15f : 1.35f, crosshairy /= _settings->GetIfEmulatorOverclocked() ? 1.15f : 1.35f;
                    guny = PluginHelpers::ClampFloat(guny, -GUNAIMLIMIT, GUNAIMLIMIT);
                    crosshairy = PluginHelpers::ClampFloat(crosshairy, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                    _link->WriteFloat(playerbase[player] + PD_gunry, guny);
                    _link->WriteFloat(playerbase[player] + PD_gunryrecoil, crosshairy * (GUNRECOILYLIMIT / CROSSHAIRLIMIT));
                    _link->WriteFloat(playerbase[player] + PD_gunly, guny);
                    _link->WriteFloat(playerbase[player] + PD_gunlyrecoil, crosshairy * (GUNRECOILYLIMIT / CROSSHAIRLIMIT));
                    _link->WriteFloat(playerbase[player] + PD_crosshairy, crosshairy);
                }
            }
        }

    }

void PerfectDark::_processFreeAimInput(int player, const js_settings::PROFILE& profile) {
    const int camera = _link->ReadInt(PD_camera);
    const int pause = _link->ReadInt(PD_pause);
    const int mppause = (_link->ReadShort(PD_mppause) & 0xFF00);

    //vec2<float> aimstickdata = _ihandler.ProcessAimStickInputForPlayer((PLAYERS) player, false);

    playerbase[player] = JOANNADATA(player);
    const int dead = _link->ReadInt(playerbase[player] + PD_deathflag);
    const int menu = _link->ReadInt(PD_menu(player));
    const int aimingflag = _link->ReadInt(playerbase[player] + PD_aimingflag);
    const int grabflag = _link->ReadInt(playerbase[player] + PD_grabflag);
    const unsigned int bikebase = _link->ReadInt(
            (unsigned int) _link->ReadInt(playerbase[player] + PD_bikeptr) + PD_bikebase);
    const int thirdperson = _link->ReadInt(playerbase[player] + PD_thirdperson);

    vec2<float> aimstickdata = (thirdperson == 1 || thirdperson == 2) ? _ihandler.ProcessAimStickInputForPlayer((PLAYERS) player, true) : _ihandler.ProcessAimStickInputForPlayer((PLAYERS) player, false);


    const int cursoraimingflag = profile.PerfectDarkAimMode && aimingflag &&
                                 _link->ReadInt(playerbase[player] + PD_currentweapon) !=
                                 50; // don't use cursoraiming when using the horizon scanner
    const float fov = _link->ReadFloat(playerbase[player] + PD_fov);
    const float basefov = fov > 60.0f ? (float) OVERRIDEFOV : 60.0f;
    // const float mouseaccel = profile.SETTINGS[ACCELERATION] ? sqrt(_cfgptr->Device[player].XPOS * _cfgptr->Device[player].XPOS + _cfgptr->Device[player].YPOS * _cfgptr->Device[player].YPOS) / TICKRATE / 12.0f * profile.SETTINGS[ACCELERATION] : 0;

    const vec2<float> sensitivity_basefactor_stick = (thirdperson == 1 || thirdperson == 2) ? _ihandler.GetBaseFactorForStickType(profile.StickMode) : _ihandler.GetGeneralBaseFactorForStick();

    const float sensitivity_stick_x = profile.AimStickSensitivity.x * sensitivity_basefactor_stick.x / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_stick_y = profile.AimStickSensitivity.y * sensitivity_basefactor_stick.y / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_gyro_x = (profile.GyroscopeSensitivity.x * GYRO_BASEFACTOR) / 40.0f; // * fmax(mouseaccel, 1);
    const float sensitivity_gyro_y = (profile.GyroscopeSensitivity.y * GYRO_BASEFACTOR) / 40.0f; // * fmax(mouseaccel, 1);


    const float gunsensitivity_stick_x = sensitivity_stick_x * (profile.Crosshair / 2.5f);
    const float gunsensitivity_stick_y = sensitivity_stick_y * (profile.Crosshair / 2.5f);
    const float gunsensitivity_gyro_x = sensitivity_gyro_x * (profile.Crosshair / 2.5f);
    const float gunsensitivity_gyro_y = sensitivity_gyro_y * (profile.Crosshair / 2.5f);

    //const float gunsensitivity_gyro_x = 100.f * basefov / 60.0f;
    //const float gunsensitivity_gyro_y = 100.f * basefov / 60.0f;

    float camx = _link->ReadFloat(playerbase[player] + PD_camx), camy = _link->ReadFloat(playerbase[player] + PD_camy);
    if (camx >= 0 && camx <= 360 && camy >= -90 && camy <= 90 && fov >= 1 && fov <= FOV_MAX && dead == 0 && menu == 1 &&
        pause == 0 && mppause == 0 && camera == 1 &&
        (grabflag == 0 || grabflag == 4 || grabflag == 3)) // if safe to inject
    {
        if (thirdperson == 1 || thirdperson ==
                                2) // if player is using the slayer/camspy, translate mouse input to analog stick and continue to next player
        {
            if (profile.StickMode != FLICK) {
                _camspyslayer(player, profile, thirdperson == 2, sensitivity_stick_x, sensitivity_stick_y);
            } else {
                _camspyslayer(player, profile, thirdperson == 2, 400 / 40.0f, 400 / 40.0f);
            }
            return;
        }
        _aimmode_free(player, profile, cursoraimingflag, fov, basefov);
        if (grabflag != 3) // if player is on foot
        {
            _crouch(player, profile);
            if (!cursoraimingflag) { // if not aiming (or pdaimmode is off)
                // Aiming mode will only use the camera to move if set to FREE. Splatoon allows X movement on the camera.
                camx += aimstickdata.x / 10.0f * sensitivity_stick_x * (fov / basefov); // regular mouselook calculation

                if(profile.FreeAiming == SPLATOON) {
                    camx += _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x * _cfgptr->DeltaTime *
                            (fov / basefov);
                }

            } else
                camx += aimx[player] * (fov / basefov); // scroll screen with aimx/aimy
            while (camx < 0)
                camx += 360;
            while (camx >= 360)
                camx -= 360;
            _link->WriteFloat(playerbase[player] + PD_camx, camx);
        } else if (_link->IsWithinRange(bikebase)) // if player is riding hoverbike (and hoverbike address is legal)
        {
            PD_ResetCrouchToggle(player);
            float bikeyaw = _link->ReadFloat(bikebase + PD_bikeyaw), bikeroll = _link->ReadFloat(
                    bikebase + PD_bikeroll);
            if (!cursoraimingflag) {
                bikeyaw -= aimstickdata.x / 10.0f * sensitivity_stick_x / (360 / BIKEXROTATIONLIMIT) * (fov / basefov);
                //bikeyaw -= _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x / (360 / BIKEXROTATIONLIMIT) * _cfgptr->DeltaTime * (fov / basefov);
                bikeroll += aimstickdata.x / 10.0f * sensitivity_stick_x * (fov / basefov) / 100;
                //bikeroll += _cfgptr->Device[player].GYRO.x / 10.0f * sensitivity_gyro_x * (fov / basefov) * _cfgptr->DeltaTime / 100;
            } else {
                bikeyaw -= aimx[player] / (360 / BIKEXROTATIONLIMIT) * (fov / basefov);
                bikeroll += aimx[player] * sensitivity_stick_x * (fov / basefov) / 100; // Use stick sensitivity (???)
            }
            while (bikeyaw < 0)
                bikeyaw += BIKEXROTATIONLIMIT;
            while (bikeyaw >= BIKEXROTATIONLIMIT)
                bikeyaw -= BIKEXROTATIONLIMIT;
            bikeroll = PluginHelpers::ClampFloat(bikeroll, -BIKEROLLLIMIT, BIKEROLLLIMIT);
            _link->WriteFloat(bikebase + PD_bikeyaw, bikeyaw);
            _link->WriteFloat(bikebase + PD_bikeroll, bikeroll);
        }
        if (!cursoraimingflag) {
            camy += (!profile.StickPitchInverted ? -aimstickdata.y : aimstickdata.y) /
                    10.0f * sensitivity_stick_y * (fov / basefov);

            float crosshairy = _link->ReadFloat(playerbase[player] + PD_crosshairy);
            if (crosshairy > 2 || crosshairy < -2) {
                camy += (!profile.GyroPitchInverted ? -_cfgptr->Device[player].GYRO.y
                                                : _cfgptr->Device[player].GYRO.y) /
                        10.0f * sensitivity_gyro_x * _cfgptr->DeltaTime * (fov / basefov);
            }
        } else {
            camy += -aimy[player] * (fov / basefov);
        }
        camy = PluginHelpers::ClampFloat(camy, -90, 90);
        _link->WriteFloat(playerbase[player] + PD_camy, camy);


        if (profile.Crosshair &&
            !cursoraimingflag) // if crosshair movement is enabled and player isn't aiming (don't calculate weapon movement while the player is in aim mode)
        {
            float gunx = _link->ReadFloat(playerbase[player] + PD_gunrx), crosshairx = _link->ReadFloat(
                    playerbase[player] +
                    PD_crosshairx); // after camera x and y have been calculated and injected, calculate the gun/reload/crosshair movement
            gunx += aimstickdata.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x * (fov / basefov) * 0.05f /
                    RATIOFACTOR;
            gunx += _cfgptr->Device[player].GYRO.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x *
                    _cfgptr->DeltaTime * (fov / basefov) * 0.05f / RATIOFACTOR;

            crosshairx += aimstickdata.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_stick_x *
                          (fov / 4 / (basefov / 4)) * 0.05f / RATIOFACTOR;

            crosshairx += _cfgptr->Device[player].GYRO.x / (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_x *
                          _cfgptr->DeltaTime * (fov / 4 / (basefov / 4)) * 0.05f / RATIOFACTOR;

            if (aimingflag) // emulate cursor moving back to the center
                gunx /= _settings->GetIfEmulatorOverclocked() ? 1.03f
                                                              : 1.07f, crosshairx /= _settings->GetIfEmulatorOverclocked()
                                                                                     ? 1.03f : 1.07f;

            gunx = PluginHelpers::ClampFloat(gunx, -GUNAIMLIMIT, GUNAIMLIMIT);
            crosshairx = PluginHelpers::ClampFloat(crosshairx, -CROSSHAIRLIMIT, CROSSHAIRLIMITFREE);
            _link->WriteFloat(playerbase[player] + PD_gunrx, gunx);
            _link->WriteFloat(playerbase[player] + PD_gunrxrecoil,
                              crosshairx * (GUNRECOILXLIMIT / CROSSHAIRLIMITFREE));
            _link->WriteFloat(playerbase[player] + PD_gunlx, gunx);
            _link->WriteFloat(playerbase[player] + PD_gunlxrecoil,
                              crosshairx * (GUNRECOILXLIMIT / CROSSHAIRLIMITFREE));
            _link->WriteFloat(playerbase[player] + PD_crosshairx, crosshairx);
            if (camy > -90 && camy < 90) // only allow player's gun to pitch within a valid range
            {
                float guny = _link->ReadFloat(playerbase[player] + PD_gunry), crosshairy = _link->ReadFloat(
                        playerbase[player] + PD_crosshairy);
                guny += (!profile.StickPitchInverted ? aimstickdata.y : -aimstickdata.y) / (!aimingflag ? 10.0f : 40.0f) *
                        gunsensitivity_stick_y * (fov / basefov) * 0.075f;
                guny += (!profile.GyroPitchInverted ? _cfgptr->Device[player].GYRO.y : -_cfgptr->Device[player].GYRO.y) /
                        (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_y * _cfgptr->DeltaTime * (fov / basefov) *
                        0.075f;

                crosshairy +=
                        (!profile.StickPitchInverted ? aimstickdata.y : -aimstickdata.y) / (!aimingflag ? 10.0f : 40.0f) *
                        gunsensitivity_stick_y * (fov / 4 / (basefov / 4)) * 0.1f;
                crosshairy +=
                        (!profile.GyroPitchInverted ? _cfgptr->Device[player].GYRO.y : -_cfgptr->Device[player].GYRO.y) /
                        (!aimingflag ? 10.0f : 40.0f) * gunsensitivity_gyro_y * _cfgptr->DeltaTime *
                        (fov / 4 / (basefov / 4)) * 0.1f;


                if (aimingflag)
                    guny /= _settings->GetIfEmulatorOverclocked() ? 1.15f
                                                                  : 1.35f, crosshairy /= _settings->GetIfEmulatorOverclocked()
                                                                                         ? 1.15f : 1.35f;
                guny = PluginHelpers::ClampFloat(guny, -GUNAIMLIMIT, GUNAIMLIMIT);
                crosshairy = PluginHelpers::ClampFloat(crosshairy, -CROSSHAIRLIMIT, CROSSHAIRLIMIT);
                _link->WriteFloat(playerbase[player] + PD_gunry, guny);
                _link->WriteFloat(playerbase[player] + PD_gunryrecoil, crosshairy * (GUNRECOILYLIMIT / CROSSHAIRLIMITFREE));
                _link->WriteFloat(playerbase[player] + PD_gunly, guny);
                _link->WriteFloat(playerbase[player] + PD_gunlyrecoil, crosshairy * (GUNRECOILYLIMIT / CROSSHAIRLIMITFREE));
                _link->WriteFloat(playerbase[player] + PD_crosshairy, crosshairy);
            }
        }
    }
}


void PerfectDark::_aimmode_free(const int player, const js_settings::PROFILE& profile, const int aimingflag, const float fov, const float basefov) {
    const float crosshairx = _link->ReadFloat(playerbase[player] + PD_crosshairx);
    const float crosshairy = _link->ReadFloat(playerbase[player] + PD_crosshairy);
    const int gunrreload = _link->ReadInt(playerbase[player] + PD_gunrstate) == 1;
    const int gunlreload = _link->ReadInt(playerbase[player] + PD_gunlstate) == 1;
    const int unarmed = _link->ReadInt(playerbase[player] + PD_currentweapon) < 2;

    const float fovratio = fov / basefov, fovmodifier = basefov / 60.f; // basefov is 60 unless override is above 60
    const float threshold = 0.72f, speed = 475.f, sensitivity = 100.f * fovmodifier, centertime = 60.f;
    // In free aim, the aimer is always unlocked and tied to the gyro exclusively.

    vec2<float> aimstickdata = _ihandler.ProcessAimStickInputForPlayer((PLAYERS)player, true);
    const vec2<float> sensitivity_basefactor_stick = _ihandler.GetGeneralBaseFactorForStick();

    if(profile.AllowStickInAimMode && aimingflag) {
        crosshairposx[player] += aimstickdata.x / 10.0f *
                                 (profile.AimStickSensitivity.x * (sensitivity_basefactor_stick.x / 2) / sensitivity /
                                  RATIOFACTOR); // fmax(mouseaccel, 1); // calculate the crosshair position
        crosshairposy[player] +=
                (!profile.StickPitchInverted ? aimstickdata.y
                                             : -aimstickdata.y) / 10.0f *
                (profile.AimStickSensitivity.y * (sensitivity_basefactor_stick.y / 2) / sensitivity); // fmax(mouseaccel, 1);
    }

    //const float mouseaccel = PROFILE[player].SETTINGS[ACCELERATION] ? sqrt(DEVICE[player].XPOS * DEVICE[player].XPOS + DEVICE[player].YPOS * DEVICE[player].YPOS) / TICKRATE / 12.0f * PROFILE[player].SETTINGS[ACCELERATION] : 0;
    if(profile.FreeAiming == FREE || (profile.FreeAiming == SPLATOON && aimingflag))
        crosshairposx[player] += _cfgptr->Device[player].GYRO.x / 10.0f * ((profile.GyroscopeSensitivity.x * GYRO_BASEFACTOR) / sensitivity / RATIOFACTOR) * _cfgptr->DeltaTime;  // * fmax(mouseaccel, 1); // calculate the crosshair position


    crosshairposy[player] += (profile.GyroPitchInverted ? -_cfgptr->Device[player].GYRO.y : _cfgptr->Device[player].GYRO.y) / 10.0f * ((profile.GyroscopeSensitivity.y * GYRO_BASEFACTOR) / sensitivity) * _cfgptr->DeltaTime;
    if(profile.FreeAiming == FREE || (profile.FreeAiming == SPLATOON && aimingflag))
        crosshairposx[player] = PluginHelpers::ClampFloat(crosshairposx[player], -CROSSHAIRLIMIT, CROSSHAIRLIMIT); // apply clamp then inject

    crosshairposy[player] = PluginHelpers::ClampFloat(crosshairposy[player], -CROSSHAIRLIMIT, CROSSHAIRLIMIT);

    _link->WriteFloat(playerbase[player] + PD_crosshairx, crosshairposx[player]);
    _link->WriteFloat(playerbase[player] + PD_crosshairy, crosshairposy[player]);

    if(profile.FreeAiming == SPLATOON && !aimingflag) {
        crosshairposx[player] = crosshairx;  // Ensure x is locked to center in Splatoon mode.
    }

    if(unarmed || gunrreload) // if unarmed or reloading right weapon, remove from gunrcenter
        gunrcenter[player] -= _settings->GetIfEmulatorOverclocked() ? 1 : 2;
    else if(gunrcenter[player] < (int)centertime) // increase gunrcenter over time until it equals centertime
        gunrcenter[player] += _settings->GetIfEmulatorOverclocked() ? 1 : 2;
    if(gunlreload) // if reloading left weapon, remove from gunlcenter
        gunlcenter[player] -= _settings->GetIfEmulatorOverclocked() ? 1 : 2;
    else if(gunlcenter[player] < (int)centertime)
        gunlcenter[player] += _settings->GetIfEmulatorOverclocked() ? 1 : 2;
    if(gunrcenter[player] < 0)
        gunrcenter[player] = 0;
    if(gunlcenter[player] < 0)
        gunlcenter[player] = 0;

    _link->WriteFloat(playerbase[player] + PD_gunrx, ((float)gunrcenter[player] / centertime) * (crosshairposx[player] * 0.75f) + fovratio - 1); // calculate and inject the gun angles
    _link->WriteFloat(playerbase[player] + PD_gunrxrecoil, crosshairposx[player] * (GUNRECOILXLIMIT / CROSSHAIRLIMIT) * fovmodifier * RATIOFACTOR); // set the recoil to the correct rotation (if we don't, then the recoil is always z axis aligned)
    _link->WriteFloat(playerbase[player] + PD_gunry, ((float)gunrcenter[player] / centertime) * (crosshairposy[player] * 0.66f) + fovratio - 1);
    _link->WriteFloat(playerbase[player] + PD_gunryrecoil, crosshairposy[player] * (GUNRECOILYLIMIT / CROSSHAIRLIMIT) * fovmodifier);
    _link->WriteFloat(playerbase[player] + PD_gunlx, ((float)gunlcenter[player] / centertime) * (crosshairposx[player] * 0.75f) + fovratio - 1);
    _link->WriteFloat(playerbase[player] + PD_gunlxrecoil, crosshairposx[player] * (GUNRECOILXLIMIT / CROSSHAIRLIMIT) * fovmodifier * RATIOFACTOR);
    _link->WriteFloat(playerbase[player] + PD_gunly, ((float)gunlcenter[player] / centertime) * (crosshairposy[player] * 0.66f) + fovratio - 1);
    _link->WriteFloat(playerbase[player] + PD_gunlyrecoil, crosshairposy[player] * (GUNRECOILYLIMIT / CROSSHAIRLIMIT) * fovmodifier);


    if(aimingflag) {
        if (crosshairx > 0 && crosshairx / CROSSHAIRLIMIT >
                              threshold) // if crosshair is within threshold of the border then calculate a linear scrolling speed and enable mouselook
            aimx[player] = (crosshairx / CROSSHAIRLIMIT - threshold) * speed * TIMESTEP;
        else if (crosshairx < 0 && crosshairx / CROSSHAIRLIMIT < -threshold)
            aimx[player] = (crosshairx / CROSSHAIRLIMIT + threshold) * speed * TIMESTEP;
        else
            aimx[player] = 0;
        if (crosshairy > 0 && crosshairy / CROSSHAIRLIMIT > threshold)
            aimy[player] = (crosshairy / CROSSHAIRLIMIT - threshold) * speed * TIMESTEP;
        else if (crosshairy < 0 && crosshairy / CROSSHAIRLIMIT < -threshold)
            aimy[player] = (crosshairy / CROSSHAIRLIMIT + threshold) * speed * TIMESTEP;
        else
            aimy[player] = 0;
    }
}

void PerfectDark::_processAnalogMovement(const int player, const js_settings::PROFILE &profile) {
    const float speedsideways = _link->ReadFloat(playerbase[player] + PD_speedsideways);
    const float speedforward = _link->ReadFloat(playerbase[player] + PD_speedforwards);

    if(speedforward != 0 || speedsideways != 0) std::cout << "Speed - LR: " << speedsideways << " FB: " << speedforward << std::endl;

}