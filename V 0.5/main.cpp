#include <iostream>
#include "window.hpp"

#undef main

void main() {
    window w("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1080, 1080);

    w.setup();
    std::cout << "Running..." << std::endl;

    while (w.running) {
        w.input();
        w.update();
        w.render();
    }

    w.clean();
}
