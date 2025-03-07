#include "aci.h"

int ACI::LoadTapeFile(std::string* path){
    tapefile = std::fstream(path->c_str(), (std::ios::in | std::ios::out | std::ios::binary));
    if(!tapefile.good()){
        return -1;
    }
    return 0;
}

int ACI::writeByteToTape(u_int8_t byte){
    tapefile << byte;
    tapefile.flush();
    return 0;
}

int ACI::writeLastByte(){
    tapefile << (u_int8_t)tapebuffer[tapeindex - 1];
    tapefile.flush();
    tapeindex = 0;
    return 0;
}

void ACI::writeBitToTape(){
    if(ram.lastRamAccess >= 0xC000 && ram.lastRamAccess <= 0xC0FF)
    {
        printf("%i\n", cpu::totalCycles - delay);
        //writing...
        if(!cooldown){
            //write 0
            if((cpu::totalCycles - delay) == 238 || (cpu::totalCycles - delay) == 226){
                bitcounter--;
            }
            //write 1
            else if((cpu::totalCycles - delay) == 473 || (cpu::totalCycles - delay) == 461){
                tapebyte |= (1 << bitcounter);
                bitcounter--;
            }

            if(bitcounter == 0){
                tapebuffer[tapeindex] = tapebyte;
                if(tapeindex > 0){ //quick fix for bit 0 "escaping" to second byte
                    tapebuffer[tapeindex - 1] += ((tapebuffer[tapeindex] & 0x0100) ? 1 : 0);
                    writeByteToTape(tapebuffer[tapeindex - 1]);
                }
                tapeindex++;
                tapebyte = 0;
                bitcounter = 8;
            }
        }
        cooldown = !cooldown;
        delay = cpu::totalCycles;
    }
}

u_int8_t ACI::SetPlaybackByte(){
    u_int8_t byte;
    tapefile.read((char*)&byte, 1);
    return byte;
}

void ACI::simulateHeader(){
    sleep(4);
    PlaybackBit(0);
    sleep(21);
    ram.promA0 = !ram.promA0;
    usleep(50);
    PlaybackBit(0);
}

void ACI::PlaybackBit(u_int8_t bit){
    //convert bit to time interval
    u_int64_t lastcycles = cpu::totalCycles;
  
    switch(bit){
        case 0:{
            while(cpu::totalCycles < lastcycles + zero0);
            ram.promA0 = !ram.promA0;
            lastcycles = cpu::totalCycles;
            while(cpu::totalCycles < lastcycles + zero0);
            ram.promA0 = !ram.promA0;
            break;
        }
        case 1:{
            while(cpu::totalCycles < lastcycles + one);
            ram.promA0 = !ram.promA0;
            lastcycles = cpu::totalCycles;
            while(cpu::totalCycles < lastcycles + one);
            ram.promA0 = !ram.promA0;
            break;
        }
    }
}

void ACI::GenerateNoise(){
    PlaybackBit(1);
    PlaybackBit(0);
}

void PlaybackThread(bool* running){
    u_int8_t byte;
    //u_int16_t startaddr = 0xe000;
    aci.simulateHeader();
    while(*running){
        if(!aci.tapefile.eof()){
            byte = aci.SetPlaybackByte();
            for(int8_t bit = 7; bit > -1; bit--){
                aci.PlaybackBit((byte >> bit) & 0x01);
            }
            //usleep(400);
            /*if(ram.memptr[startaddr] != byte){
                printf("error at %x! %x instead of %x\n", startaddr, ram.memptr[startaddr], byte);
            }*/

            //startaddr++;
        }
        else{
            aci.GenerateNoise();
        }
    }
}