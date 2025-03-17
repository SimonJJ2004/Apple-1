#pragma once
#include <stdio.h>
#include <unistd.h>
#include "display.h"
#define KBD 0xd010 //RS0 = 0 RS1 = 0
#define KBDCR 0xd011 //RS0 = 1 RS1 = 0
#define DSP 0xd002 //RS0 = 0 RS1 = 1
#define DSPCR 0xd013 //RS0 = 1 RS1 = 1


inline u_int8_t DDRA = 0;
inline u_int8_t DDRB = 0;

inline u_int8_t CRA = 0;
inline u_int8_t CRB = 0;

inline u_int8_t lastKBD = 0;
inline u_int8_t lastKBDCR = 0;
inline u_int8_t lastDSP = 0;
inline u_int8_t lastDSPCR = 0;

inline bool flash = false;

inline int pixelx = 0;
inline int pixely = 0;

void Flasher(bool* running);
void DumpTerminal(char c);
void UpdateTerminal();
void UpdatePIA();