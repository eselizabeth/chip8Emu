OBJS = src/main.cpp src/cpu.cpp src/display.cpp
OBJ_NAME = chip8
all : $(OBJS)
	g++ $(OBJS) -w -lSDL2 -o $(OBJ_NAME)