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

#define GYRO_BASEFACTOR 400.0f
#define PI 3.1415927
#define __GYRO_INJECTOR_VERSION__ "v0.1"
#define __CURRENTYEAR__ "2021"

#define CONSOLE { AllocConsole(); AttachConsole(GetCurrentProcessId()); freopen("CON", "w", stdout; )};

#define FOV_MIN 40
#define FOV_MAX 120

#include "vec2.h"


// Why change the order of CONTROLLERENUM? On controllers, players are going to be generally using the left stick to move, making the movement keys redundant.
// I keep them just in case, but i'd say a vast majority of people will be using the left stick to move.
enum CONTROLLERENUM {FIRE = 0,
                     AIM,
                     ACCEPT,
                     CANCEL,
                     START,
                     CROUCH,
                     KNEEL,
                     PREVIOUSWEAPON,
                     NEXTWEAPON,
                     FORWARDS,
                     BACKWARDS,
                     STRAFELEFT,
                     STRAFERIGHT,
                     UP,
                     DOWN,
                     LEFT,
                     RIGHT,
                     RESETGYRO,
                     TOGGLEGYRO,
                     CALIBRATEGYRO,
                     TOTALBUTTONS};

enum VECTORSETTINGSENUM {AIMDEADZONE = 0, TOTALVECTORSETTINGS};
enum CONFIGENUM {CONFIG = 0, STICKSENSITIVITYX, STICKSENSITIVITYY, GYROSENSITIVITYX, GYROSENSITIVITYY, ACCELERATION, CROSSHAIR, INVERTPITCH, CROUCHTOGGLE, GEAIMMODE, PDAIMMODE, STICKMODE, STICKAIMING, TOTALSETTINGS};
enum QUICKCONFIGENUM {DISABLED = 0, DEFAULT, CUSTOM};
enum STICKMODE {FULLSTICK = 0, XONLY, FLICK, ALLMODES};
enum PLAYERS { PLAYER1 = 0, PLAYER2, PLAYER3, PLAYER4, ALLPLAYERS};
enum CONTROLLERMODE { DISCONNECTED = 0, FULLCONTROLLER, JOYCONS };

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
    enum CONTROLLERMODE ControllerMode = DISCONNECTED;
    JSDevice PrimaryDevice = {-1, None};
    JSDevice SecondaryDevice {-1, None};
} Assignment;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct PROFILE {
    // Secondary devices are used when we are in Joycon mode.
    enum STICKMODE StickMode = FULLSTICK;
    int DS4Color = 0x000000;
    int CalibrationButton = { 0x20000 };
    int BUTTONPRIM[TOTALBUTTONS] = {0x00800, 0x00400, 0x00200, 0x00100, 0x00010, 0x02000, 0x01000, 0x04000, 0x08000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00001, 0x00002, 0x00003, 0x00004, 0x00010, 0x00000, 0x00000};
    int BUTTONSEC[TOTALBUTTONS] = {};
    // Other settings (converted from enumerator arrays
    vec2<float> AimstickDeadzone = {0.10, 0.10};
    vec2<float> MoveStickDeadzone = {0.10, 0.10};
    vec2<float> AimStickSensitivity = {1.00, 1.00};
    vec2<float> GyroscopeSensitivity = {1.00, 1.00};
    float Crosshair = {1.00};
    bool GyroPitchInverted = {false};
    bool StickPitchInverted = {false};
    bool CrouchToggle = {true};
    bool GoldeneyeAimMode = {true};
    bool PerfectDarkAimMode = {true};
    bool UseStickToAim = {false};
    bool FreeAiming = {false};
    bool AimStick = false; // True: Left, False: Right
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
