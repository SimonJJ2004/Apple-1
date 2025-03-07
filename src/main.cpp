#include "memory.h"
#include "display.h"
#include "pia.h"
#include "cpu.h"
#include <fstream>
/*
TODO: 
    Tape Interface Emulation
    better keyboard
*/

int main(int argc, char* argv[]){
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Error initializing SDL!\n");
        return -1;
    }

    std::fstream rom("/etc/Apple-1/roms/ROM.bin", std::fstream::in | std::fstream::binary);
    if(rom.fail()){
        printf("error: couldn't find rom file!\n");
        return -1;
    }
    rom.read((char*)&ram.memptr[0xFF00], 256);
    rom.close();
    /*
    std::fstream basic("/etc/Apple-1/roms/basic.rom", std::fstream::in | std::fstream::binary);
    if(basic.fail()){
        printf("error: couldn't find basic rom file!\n");
        return -1;
    }
    basic.read((char*)&ram.memptr[0xE000], 4096);
    basic.close();
    */
    std::fstream charset("/etc/Apple-1/roms/CHARSET.bin", std::fstream::in | std::fstream::binary);
    if(charset.fail()){
        printf("error: couldn't find character rom file!\n");
        return -1;
    }
    charset.read((char*)&charrom[0], 1024);
    charset.close();


    std::fstream acirom("/etc/Apple-1/roms/wozaci.bin", std::fstream::in | std::fstream::binary);
    if(acirom.fail()){
        printf("error: couldn't find tape interface rom file!\n");
        return -1;
    }
    acirom.read((char*)&ram.memptr[0xC000], 256);
    acirom.seekg(0, std::ios::beg);
    acirom.read((char*)&ram.memptr[0xC100], 256);
    acirom.close();

    bool running = true;
    CreateScreen();
    SDL_Thread* flashCursor = SDL_CreateThread((SDL_ThreadFunction)Flasher, "CurFlash", &running);
    SDL_Thread* cpuThread = SDL_CreateThread((SDL_ThreadFunction)cpu::CpuThread, "CPU", &running);
    UpdateScreen();
    running = false;
    SDL_WaitThread(flashCursor, NULL);
    SDL_WaitThread(cpuThread, NULL);
    free(ram.memptr);
    SDL_DestroyWindow(screen);
    SDL_Quit();
    return 0;
}