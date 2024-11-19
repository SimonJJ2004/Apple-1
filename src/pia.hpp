#define KBD 0xd010 //RS0 = 0 RS1 = 0
#define KBDCR 0xd011 //RS0 = 1 RS1 = 0
#define DSP 0xd012 //RS0 = 0 RS1 = 1
#define DSPCR 0xd013 //RS0 = 1 RS1 = 1


u_int8_t DDRA = 0;
u_int8_t DDRB = 0;

u_int8_t CRA = 0;
u_int8_t CRB = 0;

u_int8_t lastKBD = 0;
u_int8_t lastKBDCR = 0;
u_int8_t lastDSP = 0;
u_int8_t lastDSPCR = 0;

bool flash = false;

int pixelx = 0;
int pixely = 0;
void TypeChar(u_int8_t ascii);
void DrawChar(u_int8_t ascii, int x, int y);

void Flasher(bool* running){
    while(*running){
        flash = !flash;
        usleep(500000);
        flash = !flash;
        usleep(500000);
    }
}

void UpdateTerminal(){
    if(ram.memptr[DSP] & 0x7F && ram.memptr[DSP] != 0x7F){
        DrawChar(' ', pixelx, pixely);
        TypeChar(ram.memptr[DSP] & 0x7F);
        ram.memptr[DSP] = 0x7F;
    }
    else{
        if(flash)
            DrawChar(' ', pixelx, pixely);
        else
            DrawChar('@', pixelx, pixely);
        ram.memptr[DSP] = 0x7F;
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
                }
            }
            lastDSP = ram.memptr[DSP];
           break;
        }
        case DSPCR:{
            //write
            if(lastDSPCR != ram.memptr[DSPCR]){
                CRB = ram.memptr[DSPCR];
                lastKBDCR = ram.memptr[DSPCR];
            }
            break;
        }
        default:
            UpdateTerminal();
            break;
    }
}