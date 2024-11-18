#define STACK 0x100

//vectors
#define NMI 0xfffa
#define RESET 0xfffc
#define IRQ 0xfffe

//cpu registers
u_int8_t A;
u_int8_t X;
u_int8_t Y;

u_int16_t PC;
u_int8_t SP = 0xFF;

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
struct cpuFlags{
    u_int8_t C : 1;
    u_int8_t Z : 1;
    u_int8_t I : 1;
    u_int8_t D : 1;
    u_int8_t B : 1;    
    u_int8_t empty : 1;
    u_int8_t V : 1;
    u_int8_t N : 1; 
}PF;

int totalCycles = 0;
bool jumptaken = false;
bool stepmode = false;

u_int8_t FetchNextByte(){
    PC += 1;
    return *ram.ReadMem(PC);
}

void CPUCycle(){
    totalCycles++;
    usleep(1);       
}

void SetCarry(u_int16_t result, bool borrow = false){
    if(borrow)
        PF.C = ~((result >> 8) & 1);
    else
        PF.C = (result >> 8) & 1;
}

void SetOverflow(u_int8_t oper, int16_t result){
    if((A ^ result) & (oper ^ result) & 0x80){
        PF.V = 1;
    }
    else{
        PF.V = 0;
    }

}

void SetNZ(u_int8_t result){ //set negative and zero flag
    PF.N = (result >> 7) & 1;
    PF.Z = (result) ? 0 : 1;
}

//Instruction Handlers

void iADC(u_int8_t* oper){
    int16_t result;
    result = A + *oper + PF.C;
    SetNZ(result);
    SetOverflow(*oper, result);
    if(PF.D){
        if(((A & 0x0F) + (*oper & 0x0F) + PF.C) > 9) result += 6;
        if(result > 0x99){
            result += 96;
        }
    }
    SetCarry(result);
    A = result;
}

void iAND(u_int8_t* oper){
    u_int8_t result = A & *oper;
    SetNZ(result);
    A = result;
}

void iASL(u_int8_t* oper){
    u_int16_t result = *oper << 1;
    SetNZ(result);
    SetCarry(result);
    *oper = result;
}

void iBCC(u_int8_t* oper){
    if(!PF.C){
        PC += (int8_t)*oper;
        CPUCycle();
    }
}

void iBCS(u_int8_t* oper){
    if(PF.C){
        PC += (int8_t)*oper;
        CPUCycle();
    }
}

void iBEQ(u_int8_t* oper){
    if(PF.Z){
        PC += (int8_t)*oper;
        CPUCycle();
    }
}

void iBIT(u_int8_t* oper){
    PF.N = (*oper >> 7) & 1;
    PF.V = (*oper >> 6) & 1;
    u_int8_t result = A & *oper;
    PF.Z = (result) ? 0 : 1;
}

void iBMI(u_int8_t* oper){
    if(PF.N){
        PC += (int8_t)*oper;
        CPUCycle();
    }
}

void iBNE(u_int8_t* oper){
    if(!PF.Z){
        PC += (int8_t)*oper;
        CPUCycle();
    }
}


void iBPL(u_int8_t* oper){
    if(!PF.N){
        PC += (int8_t)*oper;
        CPUCycle();
    }
}

void iBRK(u_int8_t* oper){
    ram.WriteMem(STACK + SP, (u_int8_t)((PC + 2) >> 8));
    SP--;
    ram.WriteMem(STACK + SP, (u_int8_t)((PC + 2) & 0x00FF));
    SP--;
    ram.WriteMem(STACK + SP,  (0b00110000 | (PF.C << 0) | (PF.Z << 1) | (PF.I << 2) | (PF.D << 3) | (PF.B << 4) | (PF.empty << 5) | (PF.V << 6) | (PF.N << 7)));
    SP--;
    PF.I = 1;
    PC = *ram.ReadMem(IRQ) | (*ram.ReadMem(IRQ+1) << 8);
    jumptaken = true;
}

void iBVC(u_int8_t* oper){
    if(!PF.V){
        PC += (int8_t)*oper;
        CPUCycle();
    }
}


void iBVS(u_int8_t* oper){
    if(PF.V){
        PC += (int8_t)*oper;
        CPUCycle();
    }
}

void iCLC(u_int8_t* oper){
    PF.C = 0;
}

void iCLD(u_int8_t* oper){
    PF.D = 0;
}

void iCLI(u_int8_t* oper){
    PF.I = 0;
}

void iCLV(u_int8_t* oper){
    PF.V = 0;
}

void iCMP(u_int8_t* oper){
    u_int16_t result = A - *oper;
    SetNZ(result);
    SetCarry(result, true);
}

void iCPX(u_int8_t* oper){
    u_int16_t result = X - *oper;
    SetNZ(result);
    SetCarry(result, true);
}

void iCPY(u_int8_t* oper){
    u_int16_t result = Y - *oper;
    SetNZ(result);
    SetCarry(result, true);
}

void iDEC(u_int8_t* oper){
    *oper = *oper - 1;
    SetNZ(*oper);
}

void iDEX(u_int8_t* oper){
    X--;
    SetNZ(X);
}

void iDEY(u_int8_t* oper){
    Y--;
    SetNZ(Y);
}

void iEOR(u_int8_t* oper){
    u_int16_t result = A ^ *oper;
    SetNZ(result);
    A = result;
}

void iINC(u_int8_t* oper){
    *oper = *oper + 1;
    SetNZ(*oper);
}

void iINX(u_int8_t* oper){
    X++;
    SetNZ(X);
}

void iINY(u_int8_t* oper){
    Y++;
    SetNZ(Y);
}

void iJMP(u_int8_t* oper){
    PC = (u_int16_t)((*ram.ReadMem(PC) << 8) | (*ram.ReadMem(PC-1)));
    jumptaken = true;
}

void iJMPI(u_int8_t* oper){ //had to do this as separate function... (indirect jump)
    u_int16_t addr = (*ram.ReadMem((u_int16_t)(*ram.ReadMem(PC) << 8) | (*ram.ReadMem(PC-1)) + 1) << 8) | (*ram.ReadMem((u_int16_t)((*ram.ReadMem(PC) << 8) | (*ram.ReadMem(PC-1)))));
    PC = addr;
    jumptaken = true;
}

void iJSR(u_int8_t* oper){
    ram.WriteMem(STACK + SP, (u_int8_t)((PC) >> 8));
    SP--;
    ram.WriteMem(STACK + SP, (u_int8_t)((PC) & 0x00FF));
    SP--;
    PC = (u_int16_t)(*ram.ReadMem(PC) << 8) | (*ram.ReadMem(PC-1));
    jumptaken = true;
}

void iLDA(u_int8_t* oper){
    A = *oper;
    SetNZ(A);
}

void iLDX(u_int8_t* oper){
    X = *oper;
    SetNZ(X);
}

void iLDY(u_int8_t* oper){
    Y = *oper;
    SetNZ(Y);
}

void iLSR(u_int8_t* oper){
    PF.C = *oper & 1;
    u_int8_t result = (*oper >> 1);
    SetNZ(result);
    *oper = result;
}

void iNOP(u_int8_t* oper){

}

void iORA(u_int8_t* oper){
    u_int8_t result = A | *oper;
    SetNZ(result);
    A = result;
}

void iPHA(u_int8_t* oper){
    ram.WriteMem(STACK + SP, A);
    SP--;
}

void iPHP(u_int8_t* oper){
    u_int8_t flags = *reinterpret_cast<u_int8_t*>(&PF);
    ram.WriteMem(STACK + SP, flags);
    SP--;
}

void iPLA(u_int8_t* oper){
    SP++;
    A = *ram.ReadMem(STACK + SP);
    SetNZ(A);
}

void iPLP(u_int8_t* oper){
    SP++;
    u_int8_t flags = *ram.ReadMem(STACK + SP);
    PF.C = flags >> 0;
    PF.Z = flags >> 1;
    PF.I = flags >> 2;
    PF.D = flags >> 3;
    PF.B = 1;
    PF.empty = 1;
    PF.V = flags >> 6;
    PF.N = flags >> 7;
}

void iROL(u_int8_t* oper){
    u_int16_t result = (*oper << 1) | PF.C;
    SetNZ(result);
    SetCarry(result);
    *oper = result;
}

void iROR(u_int8_t* oper){
    u_int16_t result = (*oper >> 1) | (PF.C << 7);
    PF.C = *oper & 1;
    SetNZ(result);
    *oper = result;
}

void iRTI(u_int8_t* oper){
    SP++;
    PF = *(cpuFlags*)ram.ReadMem(STACK + SP);
    SP += 2;
    PC = (u_int16_t)((*ram.ReadMem(STACK + SP) << 8) | (*ram.ReadMem(STACK + SP-1)));
    jumptaken = true;
}

void iRTS(u_int8_t* oper){
    SP += 2;
    PC = (u_int16_t)((*ram.ReadMem(STACK + SP) << 8) | (*ram.ReadMem(STACK + SP-1)));
}

void iSBC(u_int8_t* oper){
    if(PF.D){
        u_int16_t result = A - *oper - !PF.C;
        SetNZ(result);
        SetOverflow(*oper, result);
        if(((A & 0x0F) - !PF.C) < (*oper & 0x0F)) result -= 6;
        if(result > 0x99){
            result -= 0x60;
        }
        SetCarry(result, true);
        A = result;
    }
    else{
        u_int8_t op = ~*oper;
        iADC(&op);
    }
}

void iSEC(u_int8_t* oper){
    PF.C = 1;
}

void iSED(u_int8_t* oper){
    PF.D = 1;
}

void iSEI(u_int8_t* oper){
    PF.I = 1;
}

void iSTA(u_int8_t* oper){
    *oper = A;
}

void iSTX(u_int8_t* oper){
    *oper = X;
}

void iSTY(u_int8_t* oper){
    *oper = Y;
}

void iTAX(u_int8_t* oper){
    X = A;
    SetNZ(X);
}

void iTAY(u_int8_t* oper){
    Y = A;
    SetNZ(Y);
}

void iTSX(u_int8_t* oper){
    X = SP;
    SetNZ(X);
}

void iTXA(u_int8_t* oper){
    A = X;
    SetNZ(A);
}

void iTXS(u_int8_t* oper){
    SP = X;
}

void iTYA(u_int8_t* oper){
    A = Y;
    SetNZ(A);
}


typedef void (*callback)(u_int8_t*); 
struct opcode{
    u_int8_t opcode;
    addressModes mode;
    int cycles;
    callback func;
            
};

//ABSX, ABSY, INDINDX and REL addressing modes can use 1/2 more cycles when crossing the page boundary!

opcode opcodetable[] = {
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

void ParseOpcode(){
    bool valid = false;

    u_int8_t opcode = *ram.ReadMem(PC);
    for(int o = 0; o <= 151; o++){
        if(opcodetable[o].opcode == opcode){
            valid = true;
            for(int cycle = 0; cycle < opcodetable[o].cycles; cycle++) CPUCycle();
            u_int8_t operand;
            switch(opcodetable[o].mode){
                case IMP:
                    opcodetable[o].func(nullptr);
                    break;
                case ACC:
                    opcodetable[o].func(&A);
                    break;
                case IMM:{
                    operand = FetchNextByte();
                    opcodetable[o].func(&operand);
                    break;
                }
                case ZP:{
                    opcodetable[o].func(ram.ReadMem(FetchNextByte()));
                    break;
                }
                case ZPX:{
                    opcodetable[o].func(ram.ReadMem((u_int8_t)(FetchNextByte() + X)));
                    break;
                }
                case ZPY:{
                    opcodetable[o].func(ram.ReadMem((u_int8_t)(FetchNextByte() + Y)));
                    break;
                }
                case REL:{
                    u_int16_t lastPC = PC;
                    operand = FetchNextByte();
                    opcodetable[o].func(&operand);
                    //check if page boundary was crossed;
                    if((lastPC & 0xFF00) != (PC & 0xFF00)){
                        CPUCycle();
                    }
                    break;
                }
                case ABS:{
                    opcodetable[o].func(ram.ReadMem(FetchNextByte() | (FetchNextByte() << 8)));
                    break;
                }
                case ABSX:{
                    u_int16_t addr = FetchNextByte() | (FetchNextByte() << 8);
                    if(((addr + X) & 0xFF00) != (addr & 0xFF00)) //check if page boundary was crossed;
                        CPUCycle();
                    opcodetable[o].func(ram.ReadMem(addr + X));
                    break;
                }
                case ABSY:{
                    u_int16_t addr = FetchNextByte() | (FetchNextByte() << 8);
                    if(((addr + Y) & 0xFF00) != (addr & 0xFF00)){ //check if page boundary was crossed;
                        CPUCycle();
                    }
                    opcodetable[o].func(ram.ReadMem(addr + Y));
                    break;
                }
                case IND:{
                    opcodetable[o].func(ram.ReadMem(FetchNextByte() | (FetchNextByte() << 8)));
                    break;
                }
                case INDXIND:{
                    u_int8_t ptr = FetchNextByte() + X;
                    u_int16_t addr = (*ram.ReadMem(ptr) | (*ram.ReadMem(ptr+1) << 8));
                    opcodetable[o].func(ram.ReadMem(addr));
                    break;
                }
                case INDINDX:{
                    u_int8_t ptr = FetchNextByte();
                    u_int16_t addr = (*ram.ReadMem(ptr) | (*ram.ReadMem(ptr+1) << 8));
                    if(((addr + Y) & 0xFF00) != (addr & 0xFF00)) //check if page boundary was crossed;
                        CPUCycle();
                    opcodetable[o].func(ram.ReadMem(addr + Y));
                    break;
                }
            } 
            break;
        }
    }
    if(!valid)
        printf("error: illegal opcode %x\n", opcode);
}

void RunCPU6502(bool* running){
    sleep(1); //wait for screen
    PF.I = 1;
    PF.empty = 1;
    PC = *ram.ReadMem(RESET) | (*ram.ReadMem(RESET+1) << 8);
    while(*running){
        jumptaken = false;
        //if(X >= 0x60) stepmode = true;
        if(stepmode){ 
            printf("%x: %x \n", PC, ram.memptr[PC]);
            printf("A:%x | X:%x | Y:%x | SP:%x | PC:%x\n", A, X, Y, SP, PC);
            printf("NV-BDIZC | cycles:%i\n%i%i%i%i%i%i%i%i\n", totalCycles, PF.N, PF.V, PF.empty, PF.B, PF.D, PF.I, PF.Z, PF.C);
            if(getchar() == 'q') stepmode = false;
        }
        ParseOpcode();
        UpdatePIA();
        if(!jumptaken)
            FetchNextByte();
    }
}