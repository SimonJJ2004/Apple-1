#define RAMSTART 0x0000
#define RAMSIZE 0x1000
#define ROMSTART 0xff00
#define ROMSIZE 0xff
#define PIASTART 0xD010
#define PIASIZE 0x03

class Memory{
    public:
        //last location of ram operations
        u_int16_t lastRamAccess;
        u_int8_t tempByte; //returned instead of pointer when the memory location is read only
        u_int8_t* memptr;

        bool CheckIfAddressWriteable(u_int16_t addr){
            if((addr <= RAMSIZE && addr < ROMSTART) || addr >= PIASTART && addr <= (PIASTART+PIASIZE)){
                return true;
            }
            return false;
        }

        u_int8_t* ReadMem(u_int16_t addr){
            lastRamAccess = addr;
            if(!CheckIfAddressWriteable(addr)){
                tempByte = memptr[addr];
                return &tempByte;
            }
            return &memptr[addr];
        }

        void WriteMem(u_int16_t addr, u_int8_t val){
            lastRamAccess = addr;
            if(CheckIfAddressWriteable(addr)){
                memptr[addr] = val;
            }
        }

        Memory(uint16_t ramsize = RAMSIZE){
            memptr = (u_int8_t*)malloc(UINT16_MAX + 1);
        }

};

Memory ram;