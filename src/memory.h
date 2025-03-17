#pragma once
#define RAM1START 0x0000
#define RAM1SIZE 0x1000
#define RAM2START 0xe000
#define RAM2SIZE 0x1000
#define ROMSTART 0xff00
#define ROMSIZE 0xff
#define PIASTART 0xD000
#define PIASIZE 0xFF
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

class Memory{
    public:
        //last location of ram operations
        u_int16_t lastRamAccess;
        char lastRamOperation;
        u_int8_t tempByte; //returned instead of pointer when the memory location is read only
        u_int8_t* memptr;
        u_int8_t promA0;

        bool CheckIfAddressWriteable(u_int16_t addr);

        u_int8_t* ReadMem(u_int16_t addr);

        void WriteMem(u_int16_t addr, u_int8_t val);

        Memory(u_int16_t ramsize = RAM1SIZE + RAM2SIZE);

};

inline Memory ram;