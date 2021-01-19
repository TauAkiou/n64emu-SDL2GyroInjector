//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H
#define INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H

#include <vector>
#include <map>
#include "../common.h"
#include "JoyShockDriver.h"

typedef struct {
    bool PlayerIsActive;
    int DeviceClass;
    int DeviceHandlePrimary;
    int DeviceHandleSecondary;
} DeviceAssignment;

class JoyShockDriver {
    protected:
        static JoyShockDriver* instance;
        int _devicecount = 0;
        int _windowactive = 1;
        int _initialized = 0;
        std::vector<int> *_handles = new std::vector<int>();
        std::vector<DeviceAssignment> *_playerassignment = new std::vector<PlayerAssignment>(); // Attach players to handles
    public:
        static JoyShockDriver getInstance();
        JoyShockDriver();
        void Initialize();
        void Terminate();
        int GetConnectedDeviceCount();
        int GetConnectedDS4Count();
        int GetConnectedSPCCount();
        int GetConnectedJCLCount();
        int GetConnectedJCRCount();
        void SetPlayerDS4Color(const int playernumber, const int color);
        void SetPlayerSPCJCNumber(const int playernumber, const int number);
        void CalibrateGyroscope(const int playernumber);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_JOYSHOCKDRIVER_H
