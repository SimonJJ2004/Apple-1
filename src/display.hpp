#define PIXELWIDTH 2
#define PIXELHEIGHT 2
#define SCREENW 1280 / 2
#define SCREENH 800 / 2

char charrom[1024];
SDL_Surface* surface;
int line = 0;

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

    return;
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
    usleep(1000000/30);
}


void CreateScreen(bool* running){
    SDL_Window* screen = SDL_CreateWindow("APPLE 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREENW, SCREENH, 0);

    if(screen == NULL){
        printf("Error Creating SDL Window!\n");
        return;
    }
    surface = SDL_GetWindowSurface(screen);

    SDL_Event e;
    
    while(*running){
        while(SDL_PollEvent(&e) > 0){
            switch(e.type){
                case SDL_KEYDOWN:
                {
                    u_int8_t key = (u_int8_t)((e.key.keysym.sym & ~DDRA));
                    if(e.key.keysym.sym == SDLK_F1){ //step mode
                        printf("\t****DEBUG MODE****\n");
                        printf("type h for help, q to exit\n\n");
                        cpu::stepmode = true;
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
                    *running = false;
                    break;
            }
        }
        SDL_UpdateWindowSurface(screen);
    }
    SDL_DestroyWindow(screen);
}