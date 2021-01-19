//
// Created by Robin on 1/18/2021.
//

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
        static EmulatorLink* instance;
        const unsigned char **rdramptr = nullptr; // pointer to emulator's rdram table
        const unsigned char **romptr = nullptr; // pointer to emulator's loaded rom
    public:
        static EmulatorLink* getInstance();
        EmulatorLink();
        inline short ReadShort(const unsigned int addr);
        inline void WriteShort(const unsigned int addr, const short value);
        int ReadInt(unsigned int addr);
        inline void WriteInt(const unsigned int addr, const int value);
        float ReadFloat(const unsigned int addr);
        inline void WriteFloat(const unsigned int addr, const float value);
        inline unsigned ReadROM(const unsigned int addr);
        inline void WriteROM(const unsigned int addr, const unsigned int value);
        void SetRAMPointer(const unsigned char** newramptr);
        void SetROMPointer(const unsigned char** newromptr);
};

#endif //INC_1964_INPUT_JOYSHOCKCPP_EMULATORLINK_H

