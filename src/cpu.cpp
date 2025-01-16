#include <stdint.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "cpu.hpp"

Chip8::Chip8() {
        srand(42);
        programCounter = 0x200;
        opcode = 0;
        I = 0;
        stackPointer = 0;
        delayTimer = 0;
        soundTimer = 0;
        for(int i = 0; i < DISPLAY_HEIGHT * DISPLAY_WIDTH; i++) {
            display[i] = 0;
        }
        for(int i = 0; i < 4096; i++) {
            memory[i] = 0;
        }
        for(int i = 0; i < 16; i++) {
            VN[i] = 0;
        }
        for(int i = 0; i  < 16; i++) {
            keys[i] = 0;
        }
        for(int i = 0; i  < 16; i++) {
            stack[i] = 0;
        }
    }



void Chip8::loadRom(const char* fileName) {
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
            std::cout << "Memory error" << std::endl;
        }

        size_t result = fread (buffer, 1, bufferSize, rom);
        if (result != bufferSize) 
        {
            std::cout << "Reading error" << std::endl;
        }
        if((4096-512) > bufferSize)
        {
            for(int i = 0; i < bufferSize; ++i)
                memory[i + 512] = buffer[i];
        }
        else
            std::cout << ("Error: ROM too big for memory") << std::endl;
        
        for(int i = 0; i < bufferSize; ++i) {
            memory[i + 0x200] = buffer[i];
        }
        fclose(rom);
        free(buffer);
    }


void Chip8::cycle() {
        for(int i = 0; i < instructionPerCycle; i++) {
                execute();
            }
        if (delayTimer > 0)
            delayTimer -= 1;

        if (soundTimer > 0)
            soundTimer -= 1;            
    }

void Chip8::execute() {
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
