//
// Created by Robin on 1/22/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_1964_PLUGIN_H
#define INC_1964_INPUT_JOYSHOCKCPP_1964_PLUGIN_H

#include <windows.h>
#include <plugin.h>

#define DLLEXPORT __declspec(dllexport)
#define CALL __cdecl

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
extern "C" DLLEXPORT void CALL WM_KeydwDown(WPARAM wParam, LPARAM lParam);
extern "C" DLLEXPORT void CALL WM_KeyUp(WPARAM wParam, LPARAM lParam);
extern "C" DLLEXPORT void CALL HookRDRAM(DWORD *Mem, int OCFactor);
extern "C" DLLEXPORT void CALL HookROM(DWORD *Rom);

#endif //INC_1964_INPUT_JOYSHOCKCPP_1964_PLUGIN_H
