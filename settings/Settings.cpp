//
// Created by Robin on 1/18/2021.
//

#include "Settings.h"

Controls* Controls::instance = nullptr;

Controls* Controls::getInstance() {
    if(instance == nullptr) {
        instance = new Controls();
    }
}

Controls::Controls() {

}

PluginSettings* PluginSettings::getInstance() {
    if(_instance == nullptr)
        _instance = new PluginSettings();
    return _instance;
}