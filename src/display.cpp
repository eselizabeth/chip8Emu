#include <SDL2/SDL.h>
#include <stdint.h>
#include <iostream>
#include "display.hpp"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

const int DISPLAY_HEIGHT = 32;
const int DISPLAY_WIDTH = 64;

Display::Display() {
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

void Display::destroy() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyTexture(texture);
        SDL_Quit();
    }

void Display::draw(void const* buffer) {
            SDL_UpdateTexture(texture, NULL, buffer, 64 * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
    }

bool Display::input(uint8_t* keys) {
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
            else if( event.type == SDL_KEYUP ) {
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
