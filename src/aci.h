#pragma once
#include <string>
#include <fstream>
#include "cpu.h"
#include "memory.h"
#define zero0 200
#define one 800

class ACI{
    private:
        u_int16_t tapebuffer[0xffff];
        u_int16_t tapebyte = 0;
        int tapeindex = 0;
        u_int64_t delay = 0;
        bool cooldown = false;
        int8_t bitcounter = 8;
    public:
        std::fstream tapefile;
        bool recording = false;
        bool playing = false;
        int LoadTapeFile(std::string* path);

        int writeByteToTape(u_int8_t byte);

        int writeLastByte();
        
        void writeBitToTape();

        u_int8_t SetPlaybackByte();

        void simulateHeader();

        void PlaybackBit(u_int8_t bit);

        void GenerateNoise();
};
void PlaybackThread(bool* running);

inline ACI aci;