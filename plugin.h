//
// Created by Robin on 1/18/2021.
//
#ifndef INC_1964_INPUT_JOYSHOCKCPP_PLUGIN_H
#define INC_1964_INPUT_JOYSHOCKCPP_PLUGIN_H

// N64 Emulator Plugin Specifications

#define PLUGIN_TYPE_CONTROLLER 4

#define PLUGIN_NONE 1
#define PLUGIN_MEMPACK 2
#define PLUGIN_RUMBLE_PAK 3 // Not currently supported, not sure if by emulator or no
#define PLUGIN_TRANSFER_PAK 4 // lol

// Structures (Note: BOOL, BYTE, WORD, DWORD, TRUE, FALSE are defined in windows.h)
#ifndef _WINDOWS_
typedef unsigned short WORD;
typedef int BOOL;
typedef unsigned long DWORD;
typedef void * HWND;
#endif

typedef struct
{
    WORD Version;
    WORD Type;
    char Name[100];
    BOOL Reserved1;
    BOOL Reserved2;
} PLUGIN_INFO;

typedef struct {
    BOOL Present;
    BOOL RawData;
    int Plugin;
} CONTROL;

typedef union
{
    DWORD Value;
    struct
    {
        unsigned R_DPAD: 1;
        unsigned L_DPAD: 1;
        unsigned D_DPAD: 1;
        unsigned U_DPAD: 1;
        unsigned START_BUTTON: 1;
        unsigned Z_TRIG: 1;
        unsigned B_BUTTON: 1;
        unsigned A_BUTTON: 1;

        unsigned R_CBUTTON: 1;
        unsigned L_CBUTTON: 1;
        unsigned D_CBUTTON: 1;
        unsigned U_CBUTTON: 1;
        unsigned R_TRIG: 1;
        unsigned L_TRIG: 1;
        unsigned Reserved1: 1;
        unsigned Reserved2: 1;

        signed Y_AXIS: 8;

        signed X_AXIS: 8;
    };
} BUTTONS;

static BUTTONS CONTROLLER[4];

#endif