//
// Created by Robin on 1/18/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H
#define INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H

#include "../common.h"

class Controls {
    private:
        static Controls* instance;
    public:
        static Controls* getInstance();
        PROFILE Profiles[4];
        DEVICE Devices[4];
        float DeltaTime;
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_SETTINGS_H
