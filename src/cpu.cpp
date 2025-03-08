#include "cpu.h"

#define STACK 0x100

//vectors
#define NMI 0xfffa
#define RESET 0xfffc
#define IRQ 0xfffe

u_int8_t cpu::FetchNextByte(){
    PC += 1;
    return *ram.ReadMem(PC);
}

void cpu::SetCarry(u_int16_t result, bool borrow){
    if(borrow)
        PF.C = ~((result >> 8) & 1);
    else
        PF.C = (result >> 8) & 1;
}

void cpu::SetOverflow(u_int8_t oper, int16_t result){
    if((A ^ result) & (oper ^ result) & 0x80){
        PF.V = 1;
    }
    else{
        PF.V = 0;
    }
}

void cpu::SetNZ(u_int8_t result){ //set negative and zero flag
    PF.N = (result >> 7) & 1;
    PF.Z = (result) ? 0 : 1;
}

    //Instruction Handlers

void cpu::iADC(u_int8_t* oper){
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

void cpu::iAND(u_int8_t* oper){
        u_int8_t result = A & *oper;
        SetNZ(result);
        A = result;
    }

void cpu::iASL(u_int8_t* oper){
        u_int16_t result = *oper << 1;
        SetNZ(result);
        SetCarry(result);
        *oper = result;
    }

void cpu::iBCC(u_int8_t* oper){
        if(!PF.C){
            PC += (int8_t)*oper;
            bonuscycle = true;
        }
    }

void cpu::iBCS(u_int8_t* oper){
        if(PF.C){
            PC += (int8_t)*oper;
            bonuscycle = true;
        }
    }

void cpu::iBEQ(u_int8_t* oper){
        if(PF.Z){
            PC += (int8_t)*oper;
            bonuscycle = true;
        }
    }

void cpu::iBIT(u_int8_t* oper){
        PF.N = (*oper >> 7) & 1;
        PF.V = (*oper >> 6) & 1;
        u_int8_t result = A & *oper;
        PF.Z = (result) ? 0 : 1;
    }

void cpu::iBMI(u_int8_t* oper){
        if(PF.N){
            PC += (int8_t)*oper;
            bonuscycle = true;
        }
    }

void cpu::iBNE(u_int8_t* oper){
    if(!PF.Z){
        PC += (int8_t)*oper;
        bonuscycle = true;
    }
 }


void cpu::iBPL(u_int8_t* oper){
    if(!PF.N){
        PC += (int8_t)*oper;
        bonuscycle = true;
    }
}

void cpu::iBRK(u_int8_t* oper){
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

void cpu::iBVC(u_int8_t* oper){
    if(!PF.V){
        PC += (int8_t)*oper;
        bonuscycle = true;
    }
}


void cpu::iBVS(u_int8_t* oper){
    if(PF.V){
        PC += (int8_t)*oper;
        bonuscycle = true;
    }
}

void cpu::iCLC(u_int8_t* oper){
    PF.C = 0;
}

void cpu::iCLD(u_int8_t* oper){
    PF.D = 0;
}

void cpu::iCLI(u_int8_t* oper){
    PF.I = 0;
}

void cpu::iCLV(u_int8_t* oper){
    PF.V = 0;
}

void cpu::iCMP(u_int8_t* oper){
    u_int16_t result = A - *oper;
    SetNZ(result);
    SetCarry(result, true);
}

void cpu::iCPX(u_int8_t* oper){
    u_int16_t result = X - *oper;
    SetNZ(result);
    SetCarry(result, true);
}

void cpu::iCPY(u_int8_t* oper){
    u_int16_t result = Y - *oper;
    SetNZ(result);
    SetCarry(result, true);
}

void cpu::iDEC(u_int8_t* oper){
    *oper = *oper - 1;
    SetNZ(*oper);
}

void cpu::iDEX(u_int8_t* oper){
    X--;
    SetNZ(X);
}

void cpu::iDEY(u_int8_t* oper){
    Y--;
    SetNZ(Y);
}

void cpu::iEOR(u_int8_t* oper){
    u_int16_t result = A ^ *oper;
    SetNZ(result);
    A = result;
}

void cpu::iINC(u_int8_t* oper){
    *oper = *oper + 1;
    SetNZ(*oper);
}

void cpu::iINX(u_int8_t* oper){
    X++;
    SetNZ(X);
}

void cpu::iINY(u_int8_t* oper){
    Y++;
    SetNZ(Y);
}

void cpu::iJMP(u_int8_t* oper){
    PC = (u_int16_t)((*ram.ReadMem(PC) << 8) | (*ram.ReadMem(PC-1)));
    jumptaken = true;
}

void cpu::iJMPI(u_int8_t* oper){ //had to do this as separate function... (indirect jump)
    u_int16_t addr = (*ram.ReadMem((u_int16_t)(*ram.ReadMem(PC) << 8) | (*ram.ReadMem(PC-1)) + 1) << 8) | (*ram.ReadMem((u_int16_t)((*ram.ReadMem(PC) << 8) | (*ram.ReadMem(PC-1)))));
    PC = addr;
    jumptaken = true;
}

void cpu::iJSR(u_int8_t* oper){
    ram.WriteMem(STACK + SP, (u_int8_t)((PC) >> 8));
    SP--;
    ram.WriteMem(STACK + SP, (u_int8_t)((PC) & 0x00FF));
    SP--;
    PC = (u_int16_t)(*ram.ReadMem(PC) << 8) | (*ram.ReadMem(PC-1));
    jumptaken = true;
}

void cpu::iLDA(u_int8_t* oper){
    A = *oper;
    SetNZ(A);
}

void cpu::iLDX(u_int8_t* oper){
    X = *oper;
    SetNZ(X);
}

void cpu::iLDY(u_int8_t* oper){
    Y = *oper;
    SetNZ(Y);
}

void cpu::iLSR(u_int8_t* oper){
    PF.C = *oper & 1;
    u_int8_t result = (*oper >> 1);
    SetNZ(result);
    *oper = result;
}

void cpu::iNOP(u_int8_t* oper){

}

void cpu::iORA(u_int8_t* oper){
    u_int8_t result = A | *oper;
    SetNZ(result);
    A = result;
}

void cpu::iPHA(u_int8_t* oper){
    ram.WriteMem(STACK + SP, A);
    SP--;
}

void cpu::iPHP(u_int8_t* oper){
    u_int8_t flags = *reinterpret_cast<u_int8_t*>(&PF);
    ram.WriteMem(STACK + SP, flags);
    SP--;
}

void cpu::iPLA(u_int8_t* oper){
    SP++;
    A = *ram.ReadMem(STACK + SP);
    SetNZ(A);
}

void cpu::iPLP(u_int8_t* oper){
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

void cpu::iROL(u_int8_t* oper){
    u_int16_t result = (*oper << 1) | PF.C;
    SetNZ(result);
    SetCarry(result);
    *oper = result;
}

void cpu::iROR(u_int8_t* oper){
    u_int16_t result = (*oper >> 1) | (PF.C << 7);
    PF.C = *oper & 1;
    SetNZ(result);
    *oper = result;
}

void cpu::iRTI(u_int8_t* oper){
    SP++;
    PF = *(cpuFlags*)ram.ReadMem(STACK + SP);
    SP += 2;
    PC = (u_int16_t)((*ram.ReadMem(STACK + SP) << 8) | (*ram.ReadMem(STACK + SP-1)));
    jumptaken = true;
}

void cpu::iRTS(u_int8_t* oper){
    SP += 2;
    PC = (u_int16_t)((*ram.ReadMem(STACK + SP) << 8) | (*ram.ReadMem(STACK + SP-1)));
}

void cpu::iSBC(u_int8_t* oper){
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

void cpu::iSEC(u_int8_t* oper){
    PF.C = 1;
}

void cpu::iSED(u_int8_t* oper){
    PF.D = 1;
}

void cpu::iSEI(u_int8_t* oper){
    PF.I = 1;
}

void cpu::iSTA(u_int8_t* oper){
    *oper = A;
    ram.lastRamOperation = 'W';
}

void cpu::iSTX(u_int8_t* oper){
    *oper = X;
    ram.lastRamOperation = 'W';
}

void cpu::iSTY(u_int8_t* oper){
    *oper = Y;
    ram.lastRamOperation = 'W';
}

void cpu::iTAX(u_int8_t* oper){
    X = A;
    SetNZ(X);
}

void cpu::iTAY(u_int8_t* oper){
    Y = A;
    SetNZ(Y);
}

void cpu::iTSX(u_int8_t* oper){
    X = SP;
    SetNZ(X);
}

void cpu::iTXA(u_int8_t* oper){
    A = X;
    SetNZ(A);
}

void cpu::iTXS(u_int8_t* oper){
    SP = X;
}

void cpu::iTYA(u_int8_t* oper){
    A = Y;
    SetNZ(A);
}


int cpu::ParseOpcode(){
    bool valid = false;
    int cycles = 0;
    u_int8_t opcode = *ram.ReadMem(PC);
    for(int o = 0; o <= 151; o++){
        if(opcodetable[o].opcode == opcode){
            valid = true;
            cycles += opcodetable[o].cycles;
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
                        cycles++;
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
                        cycles++;
                    opcodetable[o].func(ram.ReadMem(addr + X));
                    break;
                }
                case ABSY:{
                    u_int16_t addr = FetchNextByte() | (FetchNextByte() << 8);
                    if(((addr + Y) & 0xFF00) != (addr & 0xFF00)){ //check if page boundary was crossed;
                        cycles++;
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
                        cycles++;
                    opcodetable[o].func(ram.ReadMem(addr + Y));
                    break;
                }
            } 
            break;
        }
    }
    if(!valid)
        printf("error: illegal opcode %x\n", opcode);
    return cycles;
}

void cpu::DisplayHelp(){
    printf("----- Apple 1 Emulator by Knursoft -----\n");
    printf("List of commands:\n");
    printf("\th - display help\n");
    printf("\tq - quit debug/step mode\n");
    printf("\tl - load casette\n");
    printf("\tr - display 6502 registers\n");
    printf("\ts - step one instruction\n");
}

void cpu::DisplayRegisters(){
    printf("A:%x | X:%x | Y:%x | SP:%x | PC:%x\n", A, X, Y, SP, PC);
    printf("NV-BDIZC | cycles:%i\n%i%i%i%i%i%i%i%i\n", totalCycles, PF.N, PF.V, PF.empty, PF.B, PF.D, PF.I, PF.Z, PF.C);
}

unsigned int cpu::StepInstruction(){
    jumptaken = false;
    unsigned int cycles = ParseOpcode();
    cycles += bonuscycle ? 1 : 0;
    bonuscycle = false;
    std::this_thread::sleep_for(std::chrono::nanoseconds(cycles * 1000));
    if(!jumptaken){
        PC++;
    }
    totalCycles += cycles;
    return cycles;
}

void cpu::InitCPU6502(){
    PF.I = 1;
    PF.empty = 1;
    PC = *ram.ReadMem(RESET) | (*ram.ReadMem(RESET+1) << 8);
}

void cpu::CpuThread(bool* running){
    while(*running){
        StepInstruction();
        if(aci.recording) aci.writeBitToTape();
        UpdatePIA();    
    }
}