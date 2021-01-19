//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_COMMON_H
#define INC_1964_INPUT_JOYSHOCKCPP_COMMON_H


#define __GYRO_INJECTOR_VERSION__ "v0.1"
#define __CURRENTYEAR__ "2021"
#define CONSOLE { AllocConsole(); AttachConsole(GetCurrentProcessId()); freopen("CON", "w", stdout; )};

enum CONTROLLERENUM {FORWARDS = 0, BACKWARDS, STRAFELEFT, STRAFERIGHT, FIRE, AIM, ACCEPT, CANCEL, START, CROUCH, KNEEL, PREVIOUSWEAPON, NEXTWEAPON, UP, DOWN, LEFT, RIGHT, RESETGYRO, TOGGLEGYRO, TOTALBUTTONS};
enum CONFIGENUM {CONFIG = 0, STICKSENSITIVITYX, STICKSENSITIVITYY, GYROSENSITIVITYX, GYROSENSITIVITYY, ACCELERATION, CROSSHAIR, INVERTPITCH, CROUCHTOGGLE, GEAIMMODE, PDAIMMODE, CONMODE, HANDLE1, HANDLE2, COLOR, TOTALSETTINGS};
enum QUICKCONFIGENUM {DISABLED = 0, DEFAULT, CUSTOM};
enum PLAYERS { PLAYER1 = 0, PLAYER2, PLAYER3, PLAYER4, ALLPLAYERS};

enum JSD_ControllerType {
    JoyconLeft = 1,
    JoyconRight = 2,
    SwitchPro = 3,
    Dualshock_4 = 4
};

typedef struct {
    int Handle;
    JSD_ControllerType ConType;
    int DS4LightColor;
    int NSPlayerLight;
} JSDevice;

// Device is either 1 or 2, meaning that this value is assigned to this controller.
typedef struct {
    int Device;
    CONTROLLERENUM Button;
} ASSIGNMENT;

typedef struct {
    // Secondary devices are used when we are in Joycon mode.
    JSDevice AssignedDevicePrimary;
    JSDevice AssignedDeviceSecondary;
    ASSIGNMENT BUTTONPRIM[TOTALBUTTONS];
    ASSIGNMENT BUTTONSEC[TOTALBUTTONS];
    int SETTINGS[TOTALSETTINGS];
} PROFILE;

typedef struct {
    float AIMSTICKX, AIMSTICKY, GYROX, GYROY, LTRIGGER, RTRIGGER;
    int POSX, POSY; // Just in case we want to read keyboard/mouse support
    int BUTTONPRIM[TOTALBUTTONS];
    int ARROW[4];
    bool GYROISACTIVE;
} DEVICE;

#endif //INC_1964_INPUT_JOYSHOCKCPP_COMMON_H
