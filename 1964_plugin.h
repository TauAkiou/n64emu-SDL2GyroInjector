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
 * 1964_plugin.h - External interface for communicating with the emulator.
 */

#ifndef INC_1964_INPUT_JOYSHOCKCPP_1964_PLUGIN_H
#define INC_1964_INPUT_JOYSHOCKCPP_1964_PLUGIN_H

#include <windows.h>
#include "plugin.h"
#include "maindll.h"

extern "C" DLLEXPORT void CALL CloseDLL(void);
extern "C" DLLEXPORT void CALL ControllerCommand(int Control, BYTE *Command);
extern "C" DLLEXPORT void CALL DllAbout(HWND hParent);
extern "C" DLLEXPORT void CALL DllConfig(HWND hParent);
extern "C" DLLEXPORT void CALL DllTest(HWND hParent);
extern "C" DLLEXPORT void CALL GetDllInfo(PLUGIN_INFO *PluginInfo);
extern "C" DLLEXPORT void CALL GetKeys(int Control, BUTTONS* Keys);
extern "C" DLLEXPORT void CALL InitiateControllers(HWND hMainWindow, CONTROL Controls[4]);
extern "C" DLLEXPORT void CALL ReadController(int Control, BYTE *Command);
extern "C" DLLEXPORT void CALL RomClosed(void);
extern "C" DLLEXPORT void CALL RomOpen(void);
extern "C" DLLEXPORT void CALL WM_KeyDown(WPARAM wParam, LPARAM lParam);
extern "C" DLLEXPORT void CALL WM_KeyUp(WPARAM wParam, LPARAM lParam);
extern "C" DLLEXPORT void CALL HookRDRAM(DWORD *Mem, int OCFactor);
extern "C" DLLEXPORT void CALL HookROM(DWORD *Rom);

#endif //INC_1964_INPUT_JOYSHOCKCPP_1964_PLUGIN_H
