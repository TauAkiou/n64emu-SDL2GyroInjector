//
// Created by Robin on 1/18/2021.
//

#include "Settings.h"

Settings* Settings::GetInstance() {
    if(_instance == nullptr) {
        _instance = new Settings();
    }
    return _instance;
}

ControlState* ControlState::GetInstance() {
    if(_instance == nullptr)
        _instance = new ControlState();
    return _instance;
}
