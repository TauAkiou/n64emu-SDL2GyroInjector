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

class MainDll {
protected:
    static MainDll* instance;
    HINSTANCE _hinst = nullptr;
    std::wstring _jsonfilepath = {L'\0'};
    bool _guibusy = true;
    int _changeratio = 0;
    bool _emuoverclock = true;

    bool _configdialogisopen = false;
    bool _threadrunning = false;
    // Emulator pointers:
    const unsigned char **_rdramptr = nullptr;
    const unsigned char **_romptr = nullptr;
    CONTROL *_ctrlptr = nullptr;

    // Pointers to objects.
    Settings* _settingsptr = nullptr;
    InputHandler* _emuctrlptr = nullptr;
    JoyShockDriver* _jsdptr = nullptr;
    Game* _gameptr = nullptr;

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

static const std::string JsonFilePathDefault = ".\\plugin\\gyroinjector.json";

#endif //INC_1964_INPUT_JOYSHOCKCPP_MAINDLL_H