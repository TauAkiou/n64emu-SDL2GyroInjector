//
// Created by Robin on 1/21/2021.
//

#ifndef INC_1964_INPUT_JOYSHOCKCPP_HELPERS_H
#define INC_1964_INPUT_JOYSHOCKCPP_HELPERS_H

class PluginHelpers {
    public:
        static float ClampFloat(float value, float min, float max);
        static int ClampInt(int value, int min, int max);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_HELPERS_H
