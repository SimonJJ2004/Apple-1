#include <SDL.h>
#include <stdio.h>
#include <unistd.h>
#include "memory.hpp"
#include "pia.hpp"
#include "cpu.hpp"
#include "display.hpp"
#include <fstream>
#include <string>

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

    std::fstream rom("/etc/Apple-1/roms/ROM.bin", std::fstream::in);
    rom.read((char*)&ram.memptr[0xFF00], 256);
    rom.close();

    std::fstream basic("/etc/Apple-1/roms/basic.rom", std::fstream::in);
    basic.read((char*)&ram.memptr[0xE000], 4096);
    basic.close();

    std::fstream charset("/etc/Apple-1/roms/charset.bin", std::fstream::in);
    charset.read((char*)&charrom[0], 1024);
    charset.close();

    bool running = true;
    SDL_Thread* cpuThread = SDL_CreateThread((SDL_ThreadFunction)RunCPU6502, "MOS6502", &running);
    SDL_Thread* flashCursor = SDL_CreateThread((SDL_ThreadFunction)Flasher, "CurFlash", &running);
    CreateScreen(&running);

    SDL_WaitThread(cpuThread, NULL);
    SDL_WaitThread(flashCursor, NULL);
    SDL_Quit();
    return 0;
}