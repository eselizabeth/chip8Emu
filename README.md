##What is Chip8?
Chip8 was a VM written back in 1970s. It featured famous games such as Pacman, Space Invaders and Pong.

## How to run
### Linux
Install sdl2 (you can install sdl2 in ubuntu by typing <code>sudo apt install libsdl2-dev libsdl2-2.0-0</code>
compile the program \
<code>g++ main.cpp -o output -lSDL2</code> \
run the program by giving the path of the rom \
<code> ./output rom/BLITZ.ch8</code> \
### Windows
...

## Keyboard

Original               Emulator
Layout 

1 2 3 C                1 2 3 4
4 5 6 D      =>        Q W E R
7 8 9 E                A S D F
A 0 B F                Z X C V

## Images
![space invaders](/screenshots/sc1.png)
![space invaders](/screenshots/sc2.png)
![blitz](/screenshots/sc3.png)

## References
https://en.wikipedia.org/wiki/CHIP-8 \
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM Documentation \ \
https://github.com/kripod/chip8-roms Chip8 ROMs \
https://github.com/corax89/chip8-test-rom Test ROM for instructions can be found here \
https://github.com/loktar00/chip8/blob/master/roms/Keypad%20Test%20%5BHap%2C%202006%5D.ch8 Keyboard test rom can be found here \
