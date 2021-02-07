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

#define GYRO_BASEFACTOR 400
#define PI 3.1415927f
#define __GYRO_INJECTOR_VERSION__ "v0.1"
#define __CURRENTYEAR__ "2021"

#define CONSOLE { AllocConsole(); AttachConsole(GetCurrentProcessId()); freopen("CON", "w", stdout; )};

#define FOV_MIN 40
#define FOV_MAX 120

#include <chrono>
#include "vec2.h"

// Bitmask defines for buttons.
#define GAMEPAD_A 1
#define GAMEPAD_B 2
#define GAMEPAD_X 4
#define GAMEPAD_Y 8
#define GAMEPAD_BACK 16
#define GAMEPAD_GUIDE 32
#define GAMEPAD_START 64
#define GAMEPAD_LEFTSTICK 128
#define GAMEPAD_RIGHTSTICK 256
#define GAMEPAD_LEFTSHOULDER 512
#define GAMEPAD_RIGHTSHOULDER 1024
#define GAMEPAD_DPAD_UP 2048
#define GAMEPAD_DPAD_DOWN 4096
#define GAMEPAD_DPAD_LEFT 8192
#define GAMEPAD_DPAD_RIGHT 16384
#define GAMEPAD_MISC1 32768
#define GAMEPAD_PADDLE1 65536
#define GAMEPAD_PADDLE2 131072
#define GAMEPAD_PADDLE3 262144
#define GAMEPAD_PADDLE4 524288
#define GAMEPAD_TOUCHPAD 1048576
#define GAMEPAD_TRIGGER_LEFT 2097152
#define GAMEPAD_TRIGGER_RIGHT 4194304

#define GAMEPAD_OFFSET_A 0
#define GAMEPAD_OFFSET_B 1
#define GAMEPAD_OFFSET_X 2
#define GAMEPAD_OFFSET_Y 3
#define GAMEPAD_OFFSET_BACK 4
#define GAMEPAD_OFFSET_GUIDE 5
#define GAMEPAD_OFFSET_START 6
#define GAMEPAD_OFFSET_LEFTSTICK 7
#define GAMEPAD_OFFSET_RIGHTSTICK 8
#define GAMEPAD_OFFSET_LEFTSHOULDER 9
#define GAMEPAD_OFFSET_RIGHTSHOULDER 10
#define GAMEPAD_OFFSET_DPAD_UP 11
#define GAMEPAD_OFFSET_DPAD_DOWN 12
#define GAMEPAD_OFFSET_DPAD_LEFT 13
#define GAMEPAD_OFFSET_DPAD_RIGHT 14
#define GAMEPAD_OFFSET_MISC1 15
#define GAMEPAD_OFFSET_PADDLE1 16
#define GAMEPAD_OFFSET_PADDLE2 17
#define GAMEPAD_OFFSET_PADDLE3 18
#define GAMEPAD_OFFSET_PADDLE4 19
#define GAMEPAD_OFFSET_TOUCHPAD 20
#define GAMEPAD_OFFSET_TRIGGER_LEFT 21
#define GAMEPAD_OFFSET_TRIGGER_RIGHT 22

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
    Dualshock_4 = 4,
    Dualsense = 5
};


typedef struct clr {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct PROFILE {
    // Secondary devices are used when we are in Joycon mode.
    enum STICKMODE StickMode = FULLSTICK;
    int DS4Color = 0x000000;
    int CalibrationButton = { 0x20000 };
    int BUTTONPRIM[TOTALBUTTONS] = {GAMEPAD_TRIGGER_RIGHT, GAMEPAD_TRIGGER_LEFT, GAMEPAD_LEFTSHOULDER, GAMEPAD_RIGHTSHOULDER, GAMEPAD_START, GAMEPAD_A, GAMEPAD_B, GAMEPAD_X, GAMEPAD_Y, 0, 0, 0, 0, GAMEPAD_DPAD_UP, GAMEPAD_DPAD_DOWN, GAMEPAD_DPAD_LEFT, GAMEPAD_DPAD_RIGHT, GAMEPAD_BACK, 0x00000, GAMEPAD_MISC1};
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
    bool FreeAiming = {true};
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
} DEVICE;

#endif //INC_1964_INPUT_JOYSHOCKCPP_COMMON_H
