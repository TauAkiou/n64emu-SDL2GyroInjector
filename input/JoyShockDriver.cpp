//
// Created by Robin on 1/18/2021.
//

#include "JoyShockDriver.h"

JoyShockDriver* JoyShockDriver::instance = nullptr;

JoyShockDriver JoyShockDriver::getInstance() {
    return JoyShockDriver();
}

JoyShockDriver::JoyShockDriver() {

}

int JoyShockDriver::GetConnectedDeviceCount() {
    return 0;
}
