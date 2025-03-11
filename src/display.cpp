#include "display.h"

int DrawPixel(int x, int y, int pix){
    SDL_LockSurface(surface);
    u_int32_t* pixels = (u_int32_t*)surface->pixels;
    for(int ph = 0; ph < PIXELHEIGHT; ph++){
        for(int pw = 0; pw < PIXELWIDTH; pw++){
            pixels[(((y * surface->w) + x) + pw)] = pix;
        }
        y += 1;
    }
    SDL_UnlockSurface(surface);
    return 0;
}

void ClearScreen(){
    line = 0;
    pixelx = 0;
    pixely = 0;
    SDL_FillRect(surface, NULL, 0);
}

void ScrollScreen(){
    u_int32_t* pixels = (u_int32_t*)surface->pixels;
    memcpy(pixels, &pixels[surface->w*(PIXELHEIGHT*8)], (768*1280)-(PIXELHEIGHT*8*40));
}

void NewLine(){
    if(line < 23){
        pixely += (PIXELHEIGHT * 8);
        pixelx = 0;
        line++;
    }
    else{
        pixelx = 0;
        ScrollScreen();
    }
}

void DrawChar(u_int8_t ascii, int x, int y){
    int lastpixelx = x;
    int lastpixely = y;
    int charindex = 0x200 + (ascii - 0x40) * 8;
    for(int i = charindex; i < charindex + 8; i++){
        for(int c = 0; c < 6; c++){
            DrawPixel(x, y, ((charrom[i] >> c) & 1) ?  0xffffffff : 0);
            x += PIXELWIDTH;
        }
        x = lastpixelx;
        y += PIXELHEIGHT;
    }
    y = lastpixely;
}

void TypeChar(u_int8_t ascii){
    if(ascii == 0x0d){
        NewLine();
        return;
    }
    DrawChar(ascii, pixelx, pixely);
    pixelx += (PIXELWIDTH * 8);
    if(pixelx >= surface->w){
        NewLine();
    }
}

int CreateScreen(){
    screen = SDL_CreateWindow("APPLE 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREENW, SCREENH, 0);

    if(screen == NULL){
        printf("Error Creating SDL Window!\n");
        return -1;
    }
    surface = SDL_GetWindowSurface(screen);
    cpu::InitCPU6502();
    return 0;
}

int HandleInput(SDL_Event e){
    switch(e.type){
        case SDL_KEYDOWN:
        {
            u_int8_t key = (u_int8_t)((e.key.keysym.sym & ~DDRA));
            if(e.key.keysym.sym == SDLK_F1){ //step mode
                printf("\t****DEBUG MODE****\n");
                printf("type h for help, q to exit\n\n");
                cpu::stepmode = true;
                while(cpu::stepmode){
                    char input = getchar();
                    switch(input){
                        case 'q':
                            cpu::stepmode = false;
                            break;
                        case 'h':
                            cpu::DisplayHelp();
                            break;
                        case 'l':{
                            std::string path;
                            printf("Tape Path: ");
                            std::cin >> path;
                            if(aci.LoadTapeFile(&path) == -1){
                                printf("Error: couldn't open tape file!\n");
                                break;
                            }
                            printf("Tape Loaded Successfully.\n");
                            cpu::stepmode = false;
                            break;
                        }
                        case 'r':
                            cpu::DisplayRegisters();
                            break;
                        case 's':{
                            //todo
                            cpu::DisplayRegisters();
                            break;
                        }
                        default:
                            break;
                        }
                    }
                break;
            }
            else if(e.key.keysym.sym == SDLK_F2){
                aci.playing = !aci.playing; //press record on tape
                if(aci.playing == true){
                    printf(">>Started Playing<<\n");
                    SDL_CreateThread((SDL_ThreadFunction)PlaybackThread, "playTape", &aci.playing);
                }
                else{
                    printf(">>Stopped Playing<<\n");
                }
                break;
            }
            else if(e.key.keysym.sym == SDLK_F3){
                aci.recording = !aci.recording; //press record on tape
                if(aci.recording == true){
                    printf("*Started Recording*\n");
                }
                else{
                    aci.writeLastByte();
                    printf("*Stopped Recording*\n");
                }
                break;
            }

            if(key > 0x30 && key <= 0x39 && e.key.keysym.mod & KMOD_SHIFT){
                key -= 0x10;
            }
            else if(key == 'p' && e.key.keysym.mod & KMOD_SHIFT){
                key = '@';
            }
            else if(key == ',' && e.key.keysym.mod & KMOD_SHIFT){
                key = '<';
            }
            else if(key == '=' && e.key.keysym.mod & KMOD_SHIFT){
                key = '+';
            }
            else if(key == '.' && e.key.keysym.mod & KMOD_SHIFT){
                key = '>';
            }
            else if(key == '/' && e.key.keysym.mod & KMOD_SHIFT){
                key = '?';
            }
            else if(key >= 0x61 && key <= 0x7A){ //letters
                key -= 0x20;
            }
            else if(e.key.keysym.mod & KMOD_CTRL){
                ClearScreen();
                break;
            }
            else if(key > 0x7A){
                break;
            }
            else if(key == ';') key = ':' + 1 * (e.key.keysym.mod & KMOD_SHIFT);
            else if(key == '-') key = '-' - 2 * (e.key.keysym.mod & KMOD_SHIFT);
            else if(key == 0x08) key = '_';
            key |= 0x80;
            ram.memptr[KBD] = key; 
            lastKBD = ram.memptr[KBD];
            ram.memptr[KBDCR] |= 0x80;
            lastKBDCR |= 0x80;
            break;
        }
        case SDL_QUIT:
            return -1;
    }
    return 0;
}


int UpdateScreen(){
    SDL_Event e;
    bool running = true;
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    int fps = 60;
    
    u_int64_t lastticks = SDL_GetTicks64();
    while(running){
        if(SDL_GetTicks64() - lastticks < 1000/fps){
            cpu::StepInstruction();
            if(aci.recording) aci.writeBitToTape();
            UpdatePIA();
            continue;
        }
        lastticks = SDL_GetTicks64();
        //*** Input Handling ***
        while(SDL_PollEvent(&e) > 0){
            if(HandleInput(e) == -1){
                running = false;
            }
        }
        SDL_UpdateWindowSurface(screen);
    }
    return 0;
}