#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;
const int DISPLAY_HEIGHT = 32;
const int DISPLAY_WIDTH = 64;

struct Chip8{
    uint16_t opcode;
    uint32_t display[DISPLAY_HEIGHT * DISPLAY_WIDTH];
    uint8_t memory[4096];
    uint8_t VN[16];
    uint8_t keys[16];
    uint16_t I;
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint16_t programCounter;
    uint16_t stackPointer;
    uint32_t stack[16];
    uint8_t instructionPerCycle = 9;

    uint8_t chip8_fontset[80] =
    { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    void init(const char* fileName){
        srand(42);
        programCounter = 0x200;
        opcode = 0;
        I = 0;
        stackPointer = 0;
        delayTimer = 0;
        soundTimer = 0;
        for(int i = 0; i < DISPLAY_HEIGHT * DISPLAY_WIDTH; i++){
            display[i] = 0;
        }
        for(int i = 0; i < 4096; i++){
            memory[i] = 0;
        }
        for(int i = 0; i < 16; i++){
            VN[i] = 0;
        }
        for(int i = 0; i  < 16; i++){
            keys[i] = 0;
        }
        for(int i = 0; i  < 16; i++){
            stack[i] = 0;
        }
        loadRom(fileName);
    }

    void loadRom(const char* fileName){
        for (int i = 0; i < 80; i++) {
                memory[i] = chip8_fontset[i];
            }
        FILE *rom;
        rom = fopen(fileName, "rb");
        if ( rom == NULL )
        {
            printf( "The file failed to open." ) ;
        }
        fseek(rom , 0 , SEEK_END);
        long bufferSize = ftell(rom);
        rewind(rom);

        char * buffer = (char*)malloc(sizeof(char) * bufferSize);
        if (buffer == NULL) 
        {
            fputs ("Memory error", stderr); 
        }

        size_t result = fread (buffer, 1, bufferSize, rom);
        if (result != bufferSize) 
        {
            fputs("Reading error",stderr); 
        }
        if((4096-512) > bufferSize)
        {
            for(int i = 0; i < bufferSize; ++i)
                memory[i + 512] = buffer[i];
        }
        else
            printf("Error: ROM too big for memory");
        
        for(int i = 0; i < bufferSize; ++i){
            memory[i + 0x200] = buffer[i];
        }
        fclose(rom);
        free(buffer);
    }

    void cycle(){
        for(int i = 0; i < instructionPerCycle; i++){
                execute();
            }
        if (delayTimer > 0)
            delayTimer -= 1;

        if (soundTimer > 0)
            soundTimer -= 1;            
    }

    void execute()
    {
        opcode = (memory[programCounter] << 8) | memory[programCounter + 1];
        uint8_t x = (opcode & 0x0F00) >> 8;
        uint8_t y = (opcode & 0x00F0) >> 4;  

        switch (opcode & 0xF000)
        {
        case 0x0000:
            switch (opcode & 0x000F)
            {
            case 0x0000:
                for (int i = 0; i < DISPLAY_HEIGHT * DISPLAY_WIDTH; i++){
                    display[i] = 0;
                }
                programCounter += 2;
                break;

            case 0x000E:
                stackPointer -= 1;
                programCounter = stack[stackPointer];
                programCounter += 2;
                break;
            default:
                break;
            }
            break;

        case 0x1000:
            programCounter = (0x0FFF & opcode);
            break;

        case 0x2000:
            stack[stackPointer] = programCounter;
            stackPointer += 1;
            programCounter = (opcode & 0x0FFF);
            break;

        case 0x3000:
            if (VN[x] == (opcode & 0x00FF)){
                programCounter += 4;
            }
            else{
                programCounter += 2;
            }
            break;

        case 0x4000:
            if (VN[x] != (opcode & 0x00FF)){
                programCounter += 4;
            }
            else{
                programCounter += 2;
            }  
            break;

        case 0x5000:
            if (VN[x] == VN[y]){
                programCounter += 4;
            }
            else{
                programCounter += 2;
            }
            break;

        case 0x6000:
            VN[x] = opcode & 0x00FF;
            programCounter += 2;
            break;

        case 0x7000:
            VN[x] += opcode & 0x00FF;
            programCounter += 2;
            break;

        case 0x8000:
            switch (opcode & 0x000F)
            {
            case 0x0000:
                VN[x] = VN[y];
                programCounter += 2;
                break;

            case 0x0001:
                VN[x] |= VN[y];
                programCounter += 2;
                break;

            case 0x0002:
                VN[x] &= VN[y];
                programCounter += 2;
                break;

            case 0x0003:
                VN[x] ^= VN[y];
                programCounter += 2;
                break;

            case 0x0004:
                if(VN[x] + VN[y] > 255){
                    VN[0xF] = 1;
                }
                else{
                    VN[0xF] = 0;
                }
                VN[x] += VN[y];
                programCounter += 2;
                break;

            case 0x0005:
                if(VN[x] > VN[y]){
                    VN[0xF] = 1;
                }
                else{
                    VN[0xF] = 0;
                }
                VN[x] -= VN[y];
                programCounter += 2;
                break;

            case 0x0006:
                VN[0xF] = (VN[x] & 0x1);
                VN[x] = VN[x] >> 1;
                programCounter += 2;
                break;

            case 0x0007:
                if (VN[y] > VN[x]){
                    VN[0xF] = 1;
                }
                else{
                    VN[0xF] = 0;
                }
                VN[x] = VN[y] - VN[x];
                programCounter += 2;
                break;

            case 0x000E:
                VN[0xF] = VN[x] >> 7;
                VN[x] <<= 1;
                programCounter += 2;
                break;

            default:
                break;
            }

            break;

        case 0x9000:
            if ((VN[x]) != (VN[y])){
                programCounter += 4;
            }
            else{
                programCounter += 2;
            }
            break;

        case 0xA000:
            I = (opcode & 0x0FFF);
            programCounter += 2;
            break;

        case 0xB000:
            programCounter = (opcode & 0x0FFF) + VN[0];
            break;

        case 0xC000:
            uint8_t random; 
            random = rand() % 256;
            VN[x] = random & (opcode & 0x00FF);
            programCounter += 2;
            break;

        case 0xD000:
        {
            uint8_t n = opcode & 0x000F;
            uint8_t xPos = VN[x] % DISPLAY_WIDTH;
            uint8_t yPos = VN[y] % DISPLAY_HEIGHT;
            VN[0xF] = 0;

            for (unsigned int row = 0; row < n; row++)
            {
                uint8_t sprite = memory[I + row];

                for (unsigned int col = 0; col < 8; col++)
                {
                    uint32_t* screenPixel = &display[(yPos + row) * DISPLAY_WIDTH + (xPos + col)];
                    if (sprite & (0x80 >> col))
                    {
                        if (*screenPixel == 0xFFFFFFFF)
                        {
                            VN[0xF] = 1;
                        }
                        *screenPixel ^= 0xFFFFFFFF;
                    }
                }
            }
            programCounter += 2;
        }
        break;

        case 0xE000:
            switch (opcode & 0x00FF)
            {
            case 0x009E:
                if (keys[VN[x]] == 1){
                    programCounter += 4;
                }
                else{
                    programCounter += 2;
                }
                break;

            case 0x00A1:
                if (keys[VN[x]] == 0)
                {
                    programCounter += 4;
                }
                else{
                    programCounter += 2;
                }
                break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF)
            {
            case 0X0007:
                VN[x] = delayTimer;
                programCounter += 2;
                break;

            case 0x000A:
            {
                bool pressed = false;
                for (int i = 0; i < 16; ++i)
                {
                    if (keys[i] != 0)
                    {
                        VN[x] = i;
                        pressed = true;
                    }
                }

                if (!pressed)
                    return;
                programCounter += 2;
            }
            break;

            case 0x0015:
                delayTimer = VN[x];
                programCounter += 2;
                break;

            case 0x0018:
                soundTimer = VN[x];
                programCounter += 2;
                break;

            case 0x001E:
                I += VN[x];
                programCounter += 2;
                break;

            case 0x0029:
                I = VN[x] * 5;
                programCounter += 2;
                break;

            case 0x0033:
                memory[I] = VN[x] / 100;
                memory[I + 1] = (VN[x] / 10) % 10;
                memory[I + 2] = (VN[x]) % 10;
                programCounter += 2;
                break;

            case 0x055:
                for (int i = 0; i <= x; i++)
                {
                    memory[I + i] = VN[i];
                }
                programCounter += 2;
                break;

            case 0x0065:

                for (int i = 0; i <= x; i++)
                {
                    VN[i] = memory[I + i];
                }
                programCounter += 2;
                break;

            default:
                break;
            }
            break;
        }

        
    }
};


struct Display{
    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Texture* texture;

    void init(){
        if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) std::cout <<  "Failed at SDL_Init" << std::endl;
        window = SDL_CreateWindow("Chip8",
                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                        SCREEN_WIDTH, SCREEN_HEIGHT,
                        0);
        renderer = SDL_CreateRenderer(window, -1, 0);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        texture = SDL_CreateTexture(
           renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }

    void destroy(){
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyTexture(texture);
        SDL_Quit();
    }

    void draw(void const* buffer){
            SDL_UpdateTexture(texture, NULL, buffer, 64 * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
    }

    bool input(uint8_t* keys){
        SDL_Event event;
        bool stopEmulation = false;
         while (SDL_PollEvent(&event))
         {
            if (event.type == SDL_QUIT) stopEmulation = true;
            if( event.type == SDL_KEYDOWN ){
                switch( event.key.keysym.sym )
                {
                    case SDLK_x : keys[0] = 1; break ;
                    case SDLK_1 : keys[1] = 1; break ;
                    case SDLK_2 : keys[2] = 1; break ;
                    case SDLK_3 : keys[3] = 1; break ;
                    case SDLK_q : keys[4] = 1; break ;
                    case SDLK_w : keys[5] = 1; break ;
                    case SDLK_e : keys[6] = 1; break ;
                    case SDLK_a : keys[7] = 1; break ;
                    case SDLK_s : keys[8] = 1; break ;
                    case SDLK_d : keys[9] = 1; break ;
                    case SDLK_z : keys[10] = 1; break ;
                    case SDLK_c : keys[11] = 1; break ;
                    case SDLK_4 : keys[12] = 1; break ;
                    case SDLK_r : keys[13] = 1; break ;
                    case SDLK_f : keys[14] = 1; break ;
                    case SDLK_v : keys[15] = 1; break ;
                    default: break ;
                }
            }
            else if( event.type == SDL_KEYUP ){
                switch( event.key.keysym.sym )
                {
                    case SDLK_x : keys[0] = 0; break;
                    case SDLK_1 : keys[1] = 0; break;
                    case SDLK_2 : keys[2] = 0; break;
                    case SDLK_3 : keys[3] = 0; break;
                    case SDLK_q : keys[4] = 0; break;
                    case SDLK_w : keys[5] = 0; break;
                    case SDLK_e : keys[6] = 0; break;
                    case SDLK_a : keys[7] = 0; break;
                    case SDLK_s : keys[8] = 0; break;
                    case SDLK_d : keys[9] = 0; break;
                    case SDLK_z : keys[10] = 0; break;
                    case SDLK_c : keys[11] = 0; break ;
                    case SDLK_4 : keys[12] = 0; break;
                    case SDLK_r : keys[13] = 0; break;
                    case SDLK_f : keys[14] = 0; break;
                    case SDLK_v : keys[15] = 0; break;
                    default: break ;
                }
            }   
         }
	    return stopEmulation;
    }
};



int main(int argc, char const *argv[])
{
    if(argc == 2){
        std::cout << argv[1] << std::endl;
    }
    else{
        std::cout << "please enter only one argument as path of the ROM, ex: rom/SPACE.ch8" << std::endl;
        return 0;
    }
    int delayms = 12000;
    Chip8 chip8;
    chip8.init(argv[1]);
    Display display;
    display.init();
    bool stopEmulation = false;
    while(!stopEmulation){
        stopEmulation = display.input(chip8.keys);
        chip8.cycle();
        display.draw(chip8.display);
        usleep(delayms);
    }
    display.destroy();
    return 0;
}
