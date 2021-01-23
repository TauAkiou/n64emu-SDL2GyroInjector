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
 * emulatorlink.h - Handles memory reading/writing with the emulator's internal ROM/RAM State.
*/


#ifndef INC_1964_INPUT_JOYSHOCKCPP_EMULATORLINK_H
#define INC_1964_INPUT_JOYSHOCKCPP_EMULATORLINK_H

#define LOAD_SHALF_PARAM(param) *(short *) ((unsigned char *)rdramptr[(((param) >> 12))] + (((param) & 0x00000FFF) ^ 2))
#define LOAD_SWORD_PARAM(param) *(int *)((unsigned char *)rdramptr[(((param) >> 12))] + (((param) & 0x00000FFF)))
#define LOAD_UWORD_PARAM(param) *(unsigned int *)((unsigned char *)rdramptr[(((param) >> 12))] + (((param) & 0x00000FFF)))

#define WITHINRANGE(param) ((rdramptr != 0) && ((param & 0xFF800000U) == 0x80000000U))
#define GARBAGESHORT (short)0xABAD
#define GARBAGEINT (int)0xABADC0DE

class EmulatorLink {
    private:
        const unsigned char **rdramptr = nullptr; // pointer to emulator's rdram table
        const unsigned char **romptr = nullptr; // pointer to emulator's loaded rom
    public:
        EmulatorLink() = default;
        short ReadShort(unsigned int addr);
        void WriteShort(unsigned int addr, short value);
        int ReadInt(unsigned int addr);
        void WriteInt(unsigned int addr, int value);
        float ReadFloat(unsigned int addr);
        void WriteFloat(unsigned int addr, float value);
        unsigned ReadROM(unsigned int addr);
        void WriteROM(unsigned int addr, unsigned int value);
        bool IsWithinRange(unsigned int param);
        void SetRAMPointer(const unsigned char** newramptr);
        void SetROMPointer(const unsigned char** newromptr);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_EMULATORLINK_H

