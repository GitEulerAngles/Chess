#ifndef WINDOW
#define WINDOW

#include "SDL.h"
#include "SDL_image.h"
#include <string>
#include <iostream>

class window {
public:
    SDL_Window* screen;
    SDL_Renderer* renderer;
    bool          running = true;
    int           FPS = 60;
    int           width, height;
    float         timeMultiplier;

    window(std::string name, int x, int y, int w, int h) {
        SDL_Init(SDL_INIT_VIDEO);
        screen = SDL_CreateWindow(name.c_str(), x, y, w, h, SDL_WINDOW_SHOWN);

        if (screen == NULL) {
            running = false;
            std::cout << "Couldnt create a window\n";
        }

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cout << "SDL could not initialize!\n";
        }

        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
        {
            std::cout << "Warning: texture filtering not enabled!\n";
        }

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            std::cout << "SDL_image could not initialize!\n";
        }

        renderer = SDL_CreateRenderer(screen, 0, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

        SDL_DisplayMode Mode;
        int DisplayIndex = SDL_GetWindowDisplayIndex(screen);

        SDL_GetDesktopDisplayMode(DisplayIndex, &Mode);

        FPS = Mode.refresh_rate;


        if (renderer == NULL) {
            running = false;
            std::cout << "Couldnt create a renderer\n";
        }

        SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
        width = w;
        height = h;
        timeMultiplier = 60.0f / FPS;
    }

    ~window() {
        SDL_DestroyWindow(screen);
        SDL_DestroyRenderer(renderer);
    }

    void input();
    void update();
    void render();
    void setup();
    void clean();
};

#endif // !WINDOW
