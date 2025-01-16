#include <iostream>
#include <unistd.h>
#include "display.h"
#include "cpu.h"

int main(int argc, char const *argv[]) {
    if(argc == 2) {
        std::cout << argv[1] << std::endl;
    }
    else {
        std::cout << "please enter only one argument as path of the ROM, ex: rom/SPACE.ch8" << std::endl;
        return 0;
    }
    int delayms = 12000;
    Chip8 chip8;
    chip8.init(argv[1]);
    Display display;
    bool stopEmulation = false;
    while(!stopEmulation) {
        stopEmulation = display.input(chip8.keys);
        chip8.cycle();
        display.draw(chip8.display);
        usleep(delayms);
    }
    display.destroy();
    return 0;
}
