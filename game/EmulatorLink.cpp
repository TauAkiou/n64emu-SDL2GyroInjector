/*
 *==========================================================================
 * Joyshock Injector - 1964 Plugin
 *==========================================================================
 * Largely based off of the original Keyboard/Mouse Injector Plugin
 * Copyright (C) 2016-2021 Carnivorous
 *
 * Joyshock Injector
 * Copyright (C) 2016-2021 TauAkiou
 * All rights reserved.
 *
 * Joyshock Injector is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit http://www.gnu.org/licenses/gpl-2.0.html
 *==========================================================================
 */

/*
 * common.h - Common structures and enumerators for the plugin.
 */

#include "EmulatorLink.h"

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
    }
}

void EmulatorLink::SetRAMPointer(const unsigned char **newramptr) {
    rdramptr = newramptr;
}

void EmulatorLink::SetROMPointer(const unsigned char **newromptr) {
    romptr = newromptr;
}

bool EmulatorLink::IsWithinRange(unsigned int param) {
    return ((rdramptr != nullptr) && ((param & 0xFF800000U) == 0x80000000U));
}
