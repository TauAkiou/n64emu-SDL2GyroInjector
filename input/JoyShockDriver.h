//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H
#define INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H

#include <vector>
#include <map>
#include <windows.h>
#include <ctime>
#include "../common.h"
#include "../settings/Settings.h"
#include "../JoyShockLibrary/JoyShockLibrary.h"
#include "../game/Game.h"
#include "../common/vec2.h"

class JoyShockDriver {
    protected:
        static JoyShockDriver* instance;
        HANDLE _inputthread = nullptr;
        Controls* _ctrlptr = nullptr;
        PluginSettings* _pluginptr = nullptr;
        Game* _gameptr = nullptr;
        bool _terminatethread = true;
        int _devicecount = 0;
        int _windowactive = 1;
        int _initialized = 0;
        std::vector<JSDevice> *_devices = new std::vector<JSDevice>();
        HWND _emulatorwindow = nullptr;

        static DWORD WINAPI startinjectionloop(void* param);
        DWORD injectionloop();

        // Gyro-specific methods


    public:
        static JoyShockDriver* getInstance();
        JoyShockDriver();
        void Initialize(const HWND hw);
        void Terminate();
        void AssignEmulatorWindow(const HWND hw);

        void StartInjectionThread();
        void EndInjectionThread();
        bool IsThreadRunning();
        int GetConnectedDeviceCount() const;
        int SetPlayerHandle(PLAYERS player, int deviceclass, int phandle, int sechandle);
        std::vector<JSDevice> GetConnectedFullControllers();
        int GetConnectedDS4Count();
        int GetConnectedSPCCount();
        int GetConnectedJCLCount();
        int GetConnectedJCRCount();
        void SetPlayerDS4Color(const int playernumber, const int color);
        void SetPlayerSPCJCNumber(const int playernumber, const int number);
        void CalibrateGyroscope(const JSDevice playernumber, );
        void CalibrateAllGyroscopes();
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H
