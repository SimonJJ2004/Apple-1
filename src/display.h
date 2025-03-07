#pragma once
#include <SDL.h>
#include "pia.h"
#include "cpu.h"
#include "memory.h"
#include "aci.h"
#include <iostream>
#include <unistd.h>

#define PIXELWIDTH 2
#define PIXELHEIGHT 2
#define SCREENW 1280 / 2
#define SCREENH 800 / 2

inline char charrom[1024];
inline SDL_Surface* surface;
inline int line = 0;
inline SDL_Window* screen;

int DrawPixel(int x, int y, int pix);
void ClearScreen();
void ScrollScreen();
void NewLine();
void DrawChar(u_int8_t ascii, int x, int y);
void TypeChar(u_int8_t ascii);
int CreateScreen();
int HandleInput(SDL_Event e);
int UpdateScreen();
