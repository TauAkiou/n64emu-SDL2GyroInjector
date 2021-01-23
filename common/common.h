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

/*
 * common.h - Common structures and enumerators for the plugin.
 */

#ifndef INC_1964_INPUT_JOYSHOCKCPP_COMMON_H
#define INC_1964_INPUT_JOYSHOCKCPP_COMMON_H

#define PI 3.1415927
#define __GYRO_INJECTOR_VERSION__ "v0.1"
#define __CURRENTYEAR__ "2021"

#define CONSOLE { AllocConsole(); AttachConsole(GetCurrentProcessId()); freopen("CON", "w", stdout; )};

#define FOV_MIN 40
#define FOV_MAX 120

#include "vec2.h"


enum CONTROLLERENUM {FORWARDS = 0, BACKWARDS, STRAFELEFT, STRAFERIGHT, FIRE, AIM, ACCEPT, CANCEL, START, CROUCH, KNEEL, PREVIOUSWEAPON, NEXTWEAPON, UP, DOWN, LEFT, RIGHT, RESETGYRO, TOGGLEGYRO, TOTALBUTTONS};
enum VECTORSETTINGSENUM {AIMDEADZONE = 0, TOTALVECTORSETTINGS};
enum CONFIGENUM {CONFIG = 0, STICKSENSITIVITYX, STICKSENSITIVITYY, GYROSENSITIVITYX, GYROSENSITIVITYY, ACCELERATION, CROSSHAIR, INVERTPITCH, CROUCHTOGGLE, GEAIMMODE, PDAIMMODE, STICKMODE, STICKAIMING, TOTALSETTINGS};
enum QUICKCONFIGENUM {DISABLED = 0, DEFAULT, CUSTOM};
enum STICKMODE {FULLSTICK = 0, XONLY, FLICK, ALLMODES};
enum PLAYERS { PLAYER1 = 0, PLAYER2, PLAYER3, PLAYER4, ALLPLAYERS};
enum CONTROLLERMODE { DISCONNECTED = -1, FULLCONTROLLER, JOYCONS };

enum JSD_ControllerType {
    None = 0,
    JoyconLeft = 1,
    JoyconRight = 2,
    SwitchPro = 3,
    Dualshock_4 = 4
};

typedef struct {
    int Handle;
    JSD_ControllerType Type;
} JSDevice;

typedef struct {
    enum CONTROLLERMODE ControllerMode;
    JSDevice PrimaryDevice;
    JSDevice SecondaryDevice;
} Assignment;

typedef struct PROFILE {
    // Secondary devices are used when we are in Joycon mode.
    int ControllerMode; // -1 = Disabled, 0 = Controller, 1 = Joycon
    enum STICKMODE StickMode = FULLSTICK;
    int DS4Color = 0x000000;
    int CalibrationButton = {};
    int BUTTONPRIM[TOTALBUTTONS];
    int BUTTONSEC[TOTALBUTTONS];
    // Other settings (converted from enumerator arrays
    vec2<float> AimstickDeadzone = {};
    vec2<float> MoveStickSensitivity = {};
    vec2<float> AimStickSensitivity = {};
    vec2<float> GyroscopeSensitivity = {};
    float Crosshair = {};
    bool PitchInverted = {};
    bool CrouchToggle = {};
    bool GoldeneyeAimMode = {};
    bool PerfectDarkAimMode = {};
    bool UseStickToAim = {};
    bool FreeAiming = {};
    //vec2<float> VECTORSETTINGS[TOTALVECTORSETTINGS];
    //float FLOATSETTINGS[TOTALVECTORSETTINGS];
    //int SETTINGS[TOTALSETTINGS];
} PROFILE;

typedef struct {
    vec2<float> AIMSTICK, GYRO;
    float LTRIGGER, RTRIGGER;
    int POSX, POSY; // Just in case we want kb/m support
    int BUTTONPRIM[TOTALBUTTONS];
    int BUTTONSEC[TOTALBUTTONS];
    int ARROW[4];
    bool GYROSTATE;
    bool GYROCALIBRATION;
} DEVICE;

#endif //INC_1964_INPUT_JOYSHOCKCPP_COMMON_H
