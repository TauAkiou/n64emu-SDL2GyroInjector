#ifndef INC_1964_INPUT_JOYSHOCKCPP_MAINDLL_H
#define INC_1964_INPUT_JOYSHOCKCPP_MAINDLL_H

#include <iostream>
#include <cmath>
#include <windows.h>
#include <commctrl.h>
#include "common.h"
#include "plugin.h"
#include "game/EmulatorLink.h"
#include "input/JoyShockDriver.h"
#include "settings/Settings.h"
#include "game/Game.h"

#define DLLEXPORT __declspec(dllexport)
#define CALL __cdecl

class MainDll {
protected:
    static MainDll* instance;
    HINSTANCE _hinst = nullptr;
    std::wstring _jsonfilepath = {L'\0'};
    bool _guibusy = true;
    int _changeratio = 0;
    bool _emuoverclock = true;
    int _fovoverride = 60;
    int _overrideratiowidth = 16;
    int _overrideratioheight = 9;
    bool _showgoldeneyecrosshair = false;
    bool _configdialogisopen = false;
    bool _threadrunning = false;
    // Emulator pointers:
    const unsigned char **_rdramptr = nullptr;
    const unsigned char **_romptr = nullptr;
    CONTROL *_ctrlptr = nullptr;

    // Pointers to objects.
    Controls* _emuctrlptr = nullptr;
    JoyShockDriver* _jsdptr = nullptr;

    MainDll(HINSTANCE hinstance);
    bool Initialize(const HWND hW);
    //~MainDll();

public:
    static MainDll* getInstance(HINSTANCE hinstance = nullptr);
    void End();
    [[nodiscard]] bool IsConfigDialogOpen() const;
    void SetEmulatorOverclock(bool newoverclock);
    bool InitiateControllers(HWND window, CONTROL *ptr);
    void UpdateControllerStatus();
    void StartInjection();
    void EndInjection();

};

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

static const std::string JsonFilePathDefault = ".\\plugin\\gyroinjector.json";

#endif //INC_1964_INPUT_JOYSHOCKCPP_MAINDLL_H