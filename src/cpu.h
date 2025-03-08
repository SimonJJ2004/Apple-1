#pragma once
#include "memory.h"
#include "pia.h"
#include <unistd.h>
#include <chrono>
#include <thread>
#define STACK 0x100

//vectors
#define NMI 0xfffa
#define RESET 0xfffc
#define IRQ 0xfffe


namespace cpu{
    //cpu registers
    inline u_int8_t A;
    inline u_int8_t X;
    inline u_int8_t Y;

    inline u_int16_t PC;
    inline u_int8_t SP = 0xFF;

    enum addressModes{
        IMP = 0,
        ACC = 1,
        IMM = 2,
        ZP = 3,
        ZPX = 4,
        ZPY = 5,
        REL = 6,
        ABS = 7,
        ABSX = 8,
        ABSY = 9,
        IND = 10,
        INDXIND = 11, //Indirect X
        INDINDX = 12  //Indirect Y
    };

    //cpu flag register
    inline struct cpuFlags{
        u_int8_t C : 1;
        u_int8_t Z : 1;
        u_int8_t I : 1;
        u_int8_t D : 1;
        u_int8_t B : 1;    
        u_int8_t empty : 1;
        u_int8_t V : 1;
        u_int8_t N : 1; 
    }PF;

    inline u_int64_t totalCycles = 0;
    inline bool bonuscycle = false;
    inline bool jumptaken = false;
    inline bool stepmode = false;

    u_int8_t FetchNextByte();

    void SetCarry(u_int16_t result, bool borrow = false);

    void SetOverflow(u_int8_t oper, int16_t result);

    void SetNZ(u_int8_t result);

    //Instruction Handlers

    void iADC(u_int8_t* oper);

    void iAND(u_int8_t* oper);

    void iASL(u_int8_t* oper);

    void iBCC(u_int8_t* oper);

    void iBCS(u_int8_t* oper);

    void iBEQ(u_int8_t* oper);

    void iBIT(u_int8_t* oper);

    void iBMI(u_int8_t* oper);

    void iBNE(u_int8_t* oper);

    void iBPL(u_int8_t* oper);

    void iBRK(u_int8_t* oper);

    void iBVC(u_int8_t* oper);

    void iBVS(u_int8_t* oper);

    void iCLC(u_int8_t* oper);

    void iCLD(u_int8_t* oper);

    void iCLI(u_int8_t* oper);

    void iCLV(u_int8_t* oper);

    void iCMP(u_int8_t* oper);

    void iCPX(u_int8_t* oper);

    void iCPY(u_int8_t* oper);

    void iDEC(u_int8_t* oper);

    void iDEX(u_int8_t* oper);

    void iDEY(u_int8_t* oper);

    void iEOR(u_int8_t* oper);

    void iINC(u_int8_t* oper);

    void iINX(u_int8_t* oper);

    void iINY(u_int8_t* oper);

    void iJMP(u_int8_t* oper);

    void iJMPI(u_int8_t* oper);

    void iJSR(u_int8_t* oper);

    void iLDA(u_int8_t* oper);

    void iLDX(u_int8_t* oper);

    void iLDY(u_int8_t* oper);

    void iLSR(u_int8_t* oper);

    void iNOP(u_int8_t* oper);

    void iORA(u_int8_t* oper);

    void iPHA(u_int8_t* oper);

    void iPHP(u_int8_t* oper);

    void iPLA(u_int8_t* oper);

    void iPLP(u_int8_t* oper);

    void iROL(u_int8_t* oper);

    void iROR(u_int8_t* oper);

    void iRTI(u_int8_t* oper);

    void iRTS(u_int8_t* oper);

    void iSBC(u_int8_t* oper);

    void iSEC(u_int8_t* oper);

    void iSED(u_int8_t* oper);

    void iSEI(u_int8_t* oper);

    void iSTA(u_int8_t* oper);

    void iSTX(u_int8_t* oper);

    void iSTY(u_int8_t* oper);

    void iTAX(u_int8_t* oper);

    void iTAY(u_int8_t* oper);

    void iTSX(u_int8_t* oper);

    void iTXA(u_int8_t* oper);

    void iTXS(u_int8_t* oper);

    void iTYA(u_int8_t* oper);


    typedef void (*callback)(u_int8_t*); 
    struct opcode{
        u_int8_t opcode;
        int cycles;
        addressModes mode;
        callback func;     
    };

    //ABSX, ABSY, INDINDX and REL addressing modes can use 1/2 more cycles when crossing the page boundary!

    inline opcode opcodetable[] = {
        //ADC
        {.opcode = 0x69, .cycles = 2, .mode = IMM, .func = &iADC},
        {.opcode = 0x65, .cycles = 3, .mode = ZP, .func = &iADC},
        {.opcode = 0x75, .cycles = 4, .mode = ZPX, .func = &iADC},
        {.opcode = 0x6D, .cycles = 4, .mode = ABS, .func = &iADC},
        {.opcode = 0x7D, .cycles = 4, .mode = ABSX, .func = &iADC},
        {.opcode = 0X79, .cycles = 4, .mode = ABSY, .func = &iADC},
        {.opcode = 0x61, .cycles = 6, .mode = INDXIND, .func = &iADC},
        {.opcode = 0x71, .cycles = 5, .mode = INDINDX, .func = &iADC},
        //AND
        {.opcode = 0x29, .cycles = 2, .mode = IMM, .func = &iAND},
        {.opcode = 0x25, .cycles = 3, .mode = ZP, .func = &iAND},
        {.opcode = 0x35, .cycles = 4, .mode = ZPX, .func = &iAND},
        {.opcode = 0x2D, .cycles = 4, .mode = ABS, .func = &iAND},
        {.opcode = 0x3D, .cycles = 4, .mode = ABSX, .func = &iAND},
        {.opcode = 0X39, .cycles = 4, .mode = ABSY, .func = &iAND},
        {.opcode = 0x21, .cycles = 6, .mode = INDXIND, .func = &iAND},
        {.opcode = 0x31, .cycles = 5, .mode = INDINDX, .func = &iAND},
        //ASL
        {.opcode = 0x0A, .cycles = 2, .mode = ACC, .func = &iASL},
        {.opcode = 0x06, .cycles = 5, .mode = ZP, .func = &iASL},
        {.opcode = 0x16, .cycles = 6, .mode = ZPX, .func = &iASL},
        {.opcode = 0x0E, .cycles = 6, .mode = ABS, .func = &iASL},
        {.opcode = 0x1E, .cycles = 7, .mode = ABSX, .func = &iASL},
        //BCC
        {.opcode = 0x90, .cycles = 2, .mode = REL, .func = &iBCC},
        //BCS
        {.opcode = 0xB0, .cycles = 2, .mode = REL, .func = &iBCS},
        //BEQ
        {.opcode = 0xF0, .cycles = 2, .mode = REL, .func = &iBEQ},
        //BIT
        {.opcode = 0x24, .cycles = 3, .mode = ZP, .func = &iBIT},
        {.opcode = 0x2C, .cycles = 4, .mode = ABS, .func = &iBIT},
        //BMI
        {.opcode = 0x30, .cycles = 2, .mode = REL, .func = &iBMI},
        //BNE
        {.opcode = 0xD0, .cycles = 2, .mode = REL, .func = &iBNE},
        //BPL
        {.opcode = 0x10, .cycles = 2, .mode = REL, .func = &iBPL},
        //BRK
        {.opcode = 0x00, .cycles = 7, .mode = IMP, .func = &iBRK}, 
        //BVC
        {.opcode = 0x50, .cycles = 2, .mode = REL, .func = &iBVC}, 
        //BVS
        {.opcode = 0x70, .cycles = 2, .mode = REL, .func = &iBVS}, 
        //CLC
        {.opcode = 0x18, .cycles = 2, .mode = IMP, .func = &iCLC},
        //CLD
        {.opcode = 0xD8, .cycles = 2, .mode = IMP, .func = &iCLD},
        //CLI
        {.opcode = 0x58, .cycles = 2, .mode = IMP, .func = &iCLI},
        //CLV
        {.opcode = 0xB8, .cycles = 2, .mode = IMP, .func = &iCLV},
        //CMP
        {.opcode = 0xC9, .cycles = 2, .mode = IMM, .func = &iCMP},
        {.opcode = 0xC5, .cycles = 3, .mode = ZP, .func = &iCMP},
        {.opcode = 0xD5, .cycles = 4, .mode = ZPX, .func = &iCMP},
        {.opcode = 0xCD, .cycles = 4, .mode = ABS, .func = &iCMP},
        {.opcode = 0xDD, .cycles = 4, .mode = ABSX, .func = &iCMP},
        {.opcode = 0XD9, .cycles = 4, .mode = ABSY, .func = &iCMP},
        {.opcode = 0xC1, .cycles = 6, .mode = INDXIND, .func = &iCMP},
        {.opcode = 0xD1, .cycles = 5, .mode = INDINDX, .func = &iCMP},
        //CPX
        {.opcode = 0xE0, .cycles = 2, .mode = IMM, .func = &iCPX},
        {.opcode = 0xE4, .cycles = 3, .mode = ZP, .func = &iCPX},
        {.opcode = 0xEC, .cycles = 4, .mode = ABS, .func = &iCPX},
        //CPY
        {.opcode = 0xC0, .cycles = 2, .mode = IMM, .func = &iCPY},
        {.opcode = 0xC4, .cycles = 3, .mode = ZP, .func = &iCPY},
        {.opcode = 0xCC, .cycles = 4, .mode = ABS, .func = &iCPY},
        //DEC
        {.opcode = 0xC6, .cycles = 5, .mode = ZP, .func = iDEC},
        {.opcode = 0xD6, .cycles = 6, .mode = ZPX, .func = iDEC},
        {.opcode = 0xCE, .cycles = 6, .mode = ABS, .func = iDEC},
        {.opcode = 0xDE, .cycles = 7, .mode = ABSX, .func = iDEC},
        //DEX
        {.opcode = 0xCA, .cycles = 2, .mode = IMP, .func = iDEX},
        //DEY
        {.opcode = 0x88, .cycles = 2, .mode = IMP, .func = iDEY},
        //EOR
        {.opcode = 0x49, .cycles = 2, .mode = IMM, .func = &iEOR},
        {.opcode = 0x45, .cycles = 3, .mode = ZP, .func = &iEOR},
        {.opcode = 0x55, .cycles = 4, .mode = ZPX, .func = &iEOR},
        {.opcode = 0x4D, .cycles = 4, .mode = ABS, .func = &iEOR},
        {.opcode = 0x5D, .cycles = 4, .mode = ABSX, .func = &iEOR},
        {.opcode = 0X59, .cycles = 4, .mode = ABSY, .func = &iEOR},
        {.opcode = 0x41, .cycles = 6, .mode = INDXIND, .func = &iEOR},
        {.opcode = 0x51, .cycles = 5, .mode = INDINDX, .func = &iEOR},
        //INC
        {.opcode = 0xE6, .cycles = 5, .mode = ZP, .func = iINC},
        {.opcode = 0xF6, .cycles = 6, .mode = ZPX, .func = iINC},
        {.opcode = 0xEE, .cycles = 6, .mode = ABS, .func = iINC},
        {.opcode = 0xFE, .cycles = 7, .mode = ABSX, .func = iINC},
        //INX
        {.opcode = 0xE8, .cycles = 2, .mode = IMP, .func = iINX},
        //INY
        {.opcode = 0xC8, .cycles = 2, .mode = IMP, .func = iINY},
        //JMP
        {.opcode = 0x4C, .cycles = 3, .mode = ABS, .func = iJMP},
        {.opcode = 0x6C, .cycles = 5, .mode = IND, .func = iJMPI},
        //JSR
        {.opcode = 0x20, .cycles = 6, .mode = ABS, .func = iJSR},
        //LDA
        {.opcode = 0xA9, .cycles = 2, .mode = IMM, .func = &iLDA},
        {.opcode = 0xA5, .cycles = 3, .mode = ZP, .func = &iLDA},
        {.opcode = 0xB5, .cycles = 4, .mode = ZPX, .func = &iLDA},
        {.opcode = 0xAD, .cycles = 4, .mode = ABS, .func = &iLDA},
        {.opcode = 0xBD, .cycles = 4, .mode = ABSX, .func = &iLDA},
        {.opcode = 0xB9, .cycles = 4, .mode = ABSY, .func = &iLDA},
        {.opcode = 0xA1, .cycles = 6, .mode = INDXIND, .func = &iLDA},
        {.opcode = 0xB1, .cycles = 5, .mode = INDINDX, .func = &iLDA},
        //LDX
        {.opcode = 0xA2, .cycles = 2, .mode = IMM, .func = iLDX},
        {.opcode = 0xA6, .cycles = 3, .mode = ZP, .func = iLDX},
        {.opcode = 0xB6, .cycles = 4, .mode = ZPY, .func = iLDX},
        {.opcode = 0xAE, .cycles = 4, .mode = ABS, .func = iLDX},
        {.opcode = 0xBE, .cycles = 4, .mode = ABSY, .func = iLDX},
        //LDY
        {.opcode = 0xA0, .cycles = 2, .mode = IMM, .func = iLDY},
        {.opcode = 0xA4, .cycles = 3, .mode = ZP, .func = iLDY},
        {.opcode = 0xB4, .cycles = 4, .mode = ZPX, .func = iLDY},
        {.opcode = 0xAC, .cycles = 4, .mode = ABS, .func = iLDY},
        {.opcode = 0xBC, .cycles = 4, .mode = ABSX, .func = iLDY},
        //LSR
        {.opcode = 0x4A, .cycles = 2, .mode = ACC, .func = &iLSR},
        {.opcode = 0x46, .cycles = 5, .mode = ZP, .func = &iLSR},
        {.opcode = 0x56, .cycles = 6, .mode = ZPX, .func = &iLSR},
        {.opcode = 0x4E, .cycles = 6, .mode = ABS, .func = &iLSR},
        {.opcode = 0x5E, .cycles = 7, .mode = ABSX, .func = &iLSR},
        //NOP
        {.opcode = 0xEA, .cycles = 2, .mode = IMP, .func = &iNOP},
        //ORA
        {.opcode = 0x09, .cycles = 2, .mode = IMM, .func = &iORA},
        {.opcode = 0x05, .cycles = 3, .mode = ZP, .func = &iORA},
        {.opcode = 0x15, .cycles = 4, .mode = ZPX, .func = &iORA},
        {.opcode = 0x0D, .cycles = 4, .mode = ABS, .func = &iORA},
        {.opcode = 0x1D, .cycles = 4, .mode = ABSX, .func = &iORA},
        {.opcode = 0X19, .cycles = 4, .mode = ABSY, .func = &iORA},
        {.opcode = 0x01, .cycles = 6, .mode = INDXIND, .func = &iORA},
        {.opcode = 0x11, .cycles = 5, .mode = INDINDX, .func = &iORA},
        //PHA
        {.opcode = 0x48, .cycles = 2, .mode = IMP, .func = &iPHA},
        //PHP
        {.opcode = 0x08, .cycles = 3, .mode = IMP, .func = &iPHP},
        //PLA
        {.opcode = 0x68, .cycles = 4, .mode = IMP, .func = &iPLA},
        //PLP
        {.opcode = 0x28, .cycles = 4, .mode = IMP, .func = &iPLP},
        //ROL
        {.opcode = 0x2A, .cycles = 2, .mode = ACC, .func = &iROL},
        {.opcode = 0x26, .cycles = 5, .mode = ZP, .func = &iROL},
        {.opcode = 0x36, .cycles = 6, .mode = ZPX, .func = &iROL},
        {.opcode = 0x2E, .cycles = 6, .mode = ABS, .func = &iROL},
        {.opcode = 0x3E, .cycles = 7, .mode = ABSX, .func = &iROL},
        //ROR
        {.opcode = 0x6A, .cycles = 2, .mode = ACC, .func = &iROR},
        {.opcode = 0x66, .cycles = 5, .mode = ZP, .func = &iROR},
        {.opcode = 0x76, .cycles = 6, .mode = ZPX, .func = &iROR},
        {.opcode = 0x6E, .cycles = 6, .mode = ABS, .func = &iROR},
        {.opcode = 0x7E, .cycles = 7, .mode = ABSX, .func = &iROR},
        //RTI
        {.opcode = 0x40, .cycles = 6, .mode = IMP, .func = &iRTI},
        //RTS
        {.opcode = 0x60, .cycles = 6, .mode = IMP, .func = &iRTS},
        //ADC
        {.opcode = 0xE9, .cycles = 2, .mode = IMM, .func = &iSBC},
        {.opcode = 0xE5, .cycles = 3, .mode = ZP, .func = &iSBC},
        {.opcode = 0xF5, .cycles = 4, .mode = ZPX, .func = &iSBC},
        {.opcode = 0xED, .cycles = 4, .mode = ABS, .func = &iSBC},
        {.opcode = 0xFD, .cycles = 4, .mode = ABSX, .func = &iSBC},
        {.opcode = 0XF9, .cycles = 4, .mode = ABSY, .func = &iSBC},
        {.opcode = 0xE1, .cycles = 6, .mode = INDXIND, .func = &iSBC},
        {.opcode = 0xF1, .cycles = 5, .mode = INDINDX, .func = &iSBC},
        //SEC
        {.opcode = 0x38, .cycles = 2, .mode = IMP, .func = &iSEC},
        //SED
        {.opcode = 0xF8, .cycles = 2, .mode = IMP, .func = &iSED},
        //SEI
        {.opcode = 0x78, .cycles = 2, .mode = IMP, .func = &iSEI},
        //STA
        {.opcode = 0x85, .cycles = 3, .mode = ZP, .func = &iSTA},
        {.opcode = 0x95, .cycles = 4, .mode = ZPX, .func = &iSTA},
        {.opcode = 0x8D, .cycles = 4, .mode = ABS, .func = &iSTA},
        {.opcode = 0x9D, .cycles = 5, .mode = ABSX, .func = &iSTA},
        {.opcode = 0X99, .cycles = 5, .mode = ABSY, .func = &iSTA},
        {.opcode = 0x81, .cycles = 6, .mode = INDXIND, .func = &iSTA},
        {.opcode = 0x91, .cycles = 6, .mode = INDINDX, .func = &iSTA},
        //STX
        {.opcode = 0x86, .cycles = 3, .mode = ZP, .func = &iSTX},
        {.opcode = 0x96, .cycles = 4, .mode = ZPY, .func = &iSTX},
        {.opcode = 0x8E, .cycles = 4, .mode = ABS, .func = &iSTX},
        //STY
        {.opcode = 0x84, .cycles = 3, .mode = ZP, .func = &iSTY},
        {.opcode = 0x94, .cycles = 4, .mode = ZPX, .func = &iSTY},
        {.opcode = 0x8C, .cycles = 4, .mode = ABS, .func = &iSTY},
        //TAX
        {.opcode = 0xAA, .cycles = 2, .mode = IMP, .func = &iTAX},
        //TAY
        {.opcode = 0xA8, .cycles = 2, .mode = IMP, .func = &iTAY},
        //TSX
        {.opcode = 0xBA, .cycles = 2, .mode = IMP, .func = &iTSX},
        //TXA
        {.opcode = 0x8A, .cycles = 2, .mode = IMP, .func = &iTXA},
        //TXS
        {.opcode = 0x9A, .cycles = 2, .mode = IMP, .func = &iTXS},
        //TYA
        {.opcode = 0x98, .cycles = 2, .mode = IMP, .func = &iTYA},
    };

    int ParseOpcode();
       
    void DisplayHelp();

    void DisplayRegisters();

    unsigned int StepInstruction();

    void InitCPU6502();

    void CpuThread(bool* running);
}