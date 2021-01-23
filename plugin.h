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
 * plugin.h - 1964 plugin interface.
 */

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

class Emulator {
    public:
        inline static BUTTONS Controller[4];
};


#endif

