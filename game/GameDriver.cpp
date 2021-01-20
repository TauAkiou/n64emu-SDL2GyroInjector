//
// Created by Robin on 1/18/2021.
//

#include "GameDriver.h"

float GameDriver::ClampFloat(const float value, const float min, const float max) {
    const float test = value < min ? min : value;
    return test > max ? max : test;
}

int GameDriver::ClampInt(const int value, const int min, const int max) {
    const int test = value < min ? min : value;
    return test > max ? max : test;
}

