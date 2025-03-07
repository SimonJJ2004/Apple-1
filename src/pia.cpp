#include "pia.h"

void Flasher(bool* running){
    while(*running){
        flash = !flash;
        usleep(500000);
        flash = !flash;
        usleep(500000);
    }
}

void DumpTerminal(char c){
    DrawChar(' ', pixelx, pixely);
    TypeChar(c);
    SDL_Delay(20);
}

void UpdateTerminal(){
    if(ram.memptr[DSP] & 0x7F && ram.memptr[DSP] != 0x7F){
        char c = ram.memptr[DSP] & 0x7F;
        DumpTerminal(c);
        ram.memptr[DSP] = 0x7F;
    }
    else{
        if(flash)
            DrawChar(' ', pixelx, pixely);
        else
            DrawChar('@', pixelx, pixely);
    }
}



void UpdatePIA(){
    switch(ram.lastRamAccess){
        case KBD:{
            //write
            if(lastKBD != ram.memptr[KBD]){
                if((CRA & 4) == 0){ //bit 2 of CRA was zero (DDRA access)
                    DDRA = ram.memptr[KBD];
                }
                lastKBD = ram.memptr[KBD];
            }
            else{
                ram.memptr[KBDCR] &= 0x7F;
            }
            break;
        }
        case KBDCR:{
            //write
            if(lastKBDCR != ram.memptr[KBDCR]){
                CRA = ram.memptr[KBDCR];
                lastKBDCR = ram.memptr[KBDCR];
            }
            break;
        }
        case DSP:{
            //write
            if(lastDSP != ram.memptr[DSP]){
                if((CRB & 4) == 0){ //bit 2 of CRB was zero (DDRB access)
                    DDRB = ram.memptr[DSP];
                    CRB |= 4;
                }
            }
            lastDSP = ram.memptr[DSP];
            break;
        }
        case DSPCR:{
            //write
            if(lastDSPCR != ram.memptr[DSPCR]){
                CRB = ram.memptr[DSPCR];
                lastDSPCR = ram.memptr[DSPCR];
            }
            break;
        }
        default:
            break;
    }
    UpdateTerminal();
}