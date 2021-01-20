//
// Created by Robin on 1/18/2021.
//

#include <cstdio>
#include <iostream>
#include "EmulatorLink.h"

EmulatorLink* EmulatorLink::instance = nullptr;

EmulatorLink * EmulatorLink::getInstance() {
    if(instance == nullptr) {
        instance = new EmulatorLink();
    }
    return instance;
}

EmulatorLink::EmulatorLink() {
}

short EmulatorLink::ReadShort(const unsigned int addr) {
    if(WITHINRANGE(addr))
        return LOAD_SHALF_PARAM(addr);
    return GARBAGESHORT;
}

void EmulatorLink::WriteShort(const unsigned int addr, const short value) {
    if(WITHINRANGE(addr))
        LOAD_SHALF_PARAM(addr) = value;
}

int EmulatorLink::ReadInt(const unsigned int addr) {
    if(WITHINRANGE(addr))
        return LOAD_SWORD_PARAM(addr);
    return GARBAGEINT;
}

void EmulatorLink::WriteInt(const unsigned int addr, const int value) {
    if(WITHINRANGE(addr))
        LOAD_SWORD_PARAM(addr) = value;
}

float EmulatorLink::ReadFloat(const unsigned int addr) {
    if(WITHINRANGE(addr))
        return *((float *)&LOAD_UWORD_PARAM(addr));
    return GARBAGEINT;
}

void EmulatorLink::WriteFloat(const unsigned int addr, const float value) {
    if(WITHINRANGE(addr))
        LOAD_UWORD_PARAM(addr) = *(unsigned int *)(&value);
}

unsigned EmulatorLink::ReadROM(const unsigned int addr) {
    return (unsigned)romptr[addr / 0x4];
}

void EmulatorLink::WriteROM(const unsigned int addr, const unsigned int value) {
    if(romptr != nullptr) {
        romptr[addr / 0x4] = static_cast<const unsigned char *>((void *) value);
        std::cout << "Wrote " << value << " to ROM addr " << addr << std::endl;
    }
}

void EmulatorLink::SetRAMPointer(const unsigned char **newramptr) {
    rdramptr = newramptr;
}

void EmulatorLink::SetROMPointer(const unsigned char **newromptr) {
    romptr = newromptr;
}
