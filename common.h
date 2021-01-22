//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_COMMON_H
#define INC_1964_INPUT_JOYSHOCKCPP_COMMON_H

#define PI 3.1415927
#define __GYRO_INJECTOR_VERSION__ "v0.1"
#define __CURRENTYEAR__ "2021"
#define CONSOLE { AllocConsole(); AttachConsole(GetCurrentProcessId()); freopen("CON", "w", stdout; )};

#define FOV_MIN 40
#define FOV_MAX 120

#include "common/vec2.h"


enum CONTROLLERENUM {FORWARDS = 0, BACKWARDS, STRAFELEFT, STRAFERIGHT, FIRE, AIM, ACCEPT, CANCEL, START, CROUCH, KNEEL, PREVIOUSWEAPON, NEXTWEAPON, UP, DOWN, LEFT, RIGHT, RESETGYRO, TOGGLEGYRO, TOTALBUTTONS};
enum VECTORSETTINGSENUM {AIMDEADZONE = 0, TOTALVECTORSETTINGS};
enum CONFIGENUM {CONFIG = 0, STICKSENSITIVITYX, STICKSENSITIVITYY, GYROSENSITIVITYX, GYROSENSITIVITYY, ACCELERATION, CROSSHAIR, INVERTPITCH, CROUCHTOGGLE, GEAIMMODE, PDAIMMODE, STICKMODE, STICKAIMING, TOTALSETTINGS};
enum QUICKCONFIGENUM {DISABLED = 0, DEFAULT, CUSTOM};
enum STICKMODE {FULL = 0, XONLY, FLICK, ALLMODES};
enum PLAYERS { PLAYER1 = 0, PLAYER2, PLAYER3, PLAYER4, ALLPLAYERS};

enum JSD_ControllerType {
    None = 0,
    JoyconLeft = 1,
    JoyconRight = 2,
    SwitchPro = 3,
    Dualshock_4 = 4
};

typedef struct  {
    int Handle;
    JSD_ControllerType ConType;
    int DS4LightColor;
    int NSPlayerLight;
} JSDevice;

typedef struct PROFILE {
    // Secondary devices are used when we are in Joycon mode.
    int ControllerMode; // -1 = Disabled, 0 = Controller, 1 = Joycon
    JSDevice AssignedDevicePrimary = {-1, None, 0, 0};
    JSDevice AssignedDeviceSecondary = { -1, None, 0, 0};
    QUICKCONFIGENUM QuickConfigSetting = DISABLED;
    enum STICKMODE StickMode = FULL;
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
