//
// Created by Robin on 1/21/2021.
//

#include "Helpers.h"

float PluginHelpers::ClampFloat(const float value, const float min, const float max)
{
    const float test = value < min ? min : value;
    return test > max ? max : test;
}

int PluginHelpers::ClampInt(const int value, const int min, const int max)
{
    const int test = value < min ? min : value;
    return test > max ? max : test;
}