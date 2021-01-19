//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H
#define INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H

#include <vector>
#include <map>
#include <windows.h>
#include "../common.h"
#include "../settings/Settings.h"
#include "../JoyShockLibrary/JoyShockLibrary.h"

class JoyShockDriver {
    protected:
        static JoyShockDriver* instance;
        HANDLE _inputthread = nullptr;
        Controls* _ctrlptr = nullptr;
        int _devicecount = 0;
        int _windowactive = 1;
        int _initialized = 0;
        std::vector<JSDevice> *_devices = new std::vector<JSDevice>();
        HWND _emulatorwindow = nullptr;

        static DWORD WINAPI startinjectionloop(void* param);
        DWORD injectionloop();
    public:
        static JoyShockDriver* getInstance();
        JoyShockDriver();
        void Initialize(const HWND hw);
        void Terminate();
        void AssignEmulatorWindow(const HWND hw);

        void StartInjectionThread();
        void EndInjectionThread();
        bool IsThreadRunning();
        int GetConnectedDeviceCount();
        int SetPlayerHandle(PLAYERS player, int deviceclass, int phandle, int sechandle);
        int GetConnectedDS4Count();
        int GetConnectedSPCCount();
        int GetConnectedJCLCount();
        int GetConnectedJCRCount();
        void SetPlayerDS4Color(const int playernumber, const int color);
        void SetPlayerSPCJCNumber(const int playernumber, const int number);
        void CalibrateGyroscope(const int playernumber);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H
