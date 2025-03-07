#include "memory.h"

bool Memory::CheckIfAddressWriteable(u_int16_t addr){
    if((addr < RAM1SIZE && addr < ROMSTART) || addr >= PIASTART && addr <= (PIASTART+PIASIZE) || addr >= RAM2START && addr <= (RAM2START+RAM2SIZE)){
        return true;
    }
    return false;
}
u_int8_t* Memory::ReadMem(u_int16_t addr){
    if(addr >= 0xC081 && addr <= 0xC0FF)
    {
        addr &= (0xfffe + promA0);
    }
    lastRamAccess = addr;
    lastRamOperation = 'R';
    if(!CheckIfAddressWriteable(addr)){
        tempByte = memptr[addr];
        return &tempByte;
    }
    return &memptr[addr];
}

void Memory::WriteMem(u_int16_t addr, u_int8_t val){
    lastRamAccess = addr;
    lastRamOperation = 'W';
    if(CheckIfAddressWriteable(addr)){
        memptr[addr] = val;
    }
}

Memory::Memory(u_int16_t ramsize){
    promA0 = 1;
    memptr = (u_int8_t*)malloc(UINT16_MAX + 1);
}