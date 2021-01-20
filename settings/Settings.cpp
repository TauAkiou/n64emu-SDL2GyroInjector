//
// Created by Robin on 1/18/2021.
//

#include "Settings.h"

Controls* Controls::_instance = nullptr;
PluginSettings* PluginSettings::_instance = nullptr;

Controls* Controls::getInstance() {
    if(_instance == nullptr) {
        _instance = new Controls();
    }
    return _instance;
}

PluginSettings* PluginSettings::getInstance() {
    if(_instance == nullptr)
        _instance = new PluginSettings();
    return _instance;
}
