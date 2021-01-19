//
// Created by Robin on 1/18/2021.
//

#include "JoyShockDriver.h"

JoyShockDriver* JoyShockDriver::instance = nullptr;

JoyShockDriver* JoyShockDriver::getInstance() {
    if(!instance)
        instance = new JoyShockDriver();
    return instance;
}

JoyShockDriver::JoyShockDriver() {

}

DWORD JoyShockDriver::injectionloop() {

}

int JoyShockDriver::GetConnectedDeviceCount() {
    return _devicecount;
}

void JoyShockDriver::Initialize(const HWND hw) {
    if(!_initialized) {
        _devicecount = JslConnectDevices();

        int discoveredhandles[_devicecount];
        JslGetConnectedDeviceHandles(discoveredhandles, _devicecount);

        JSDevice deviceentry;

        for(int _handleid : discoveredhandles) {
            deviceentry.Handle = _handleid;
            deviceentry.ConType = (JSD_ControllerType)JslGetControllerType(_handleid);
            deviceentry.DS4LightColor = 0x000000;
            deviceentry.NSPlayerLight = 0;

            _devices->push_back(deviceentry);
        }
        _initialized = true;
    }

}

void JoyShockDriver::AssignEmulatorWindow(const HWND hw) {
    if(_emulatorwindow == nullptr) {
        _emulatorwindow = hw;
    }
}

DWORD JoyShockDriver::startinjectionloop(void* param) {
    JoyShockDriver* ThisClass = (JoyShockDriver*) param;
    return ThisClass->injectionloop();
}


void JoyShockDriver::StartInjectionThread() {
    if(_inputthread == nullptr) {
        _inputthread = CreateThread(NULL, 0, startinjectionloop, NULL, 0, NULL);
    }
}
