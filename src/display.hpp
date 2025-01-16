#include <SDL2/SDL.h>
#include <stdint.h>

class Display{
    private:
    SDL_Renderer* renderer = NULL;
    SDL_Window* window = NULL;
    SDL_Texture* texture;
    public:
    Display();
    void destroy();
    void draw(void const* buffer);
    bool input(uint8_t* keys);
};
