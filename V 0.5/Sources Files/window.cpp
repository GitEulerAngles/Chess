#include "window.hpp"
#include "chess.h"

chess::userData* User = new chess::userData;
chess::gameData* Board = new chess::gameData;

void window::input() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_WINDOWEVENT) {
            switch (e.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                running = false;
                break;
            }
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            switch (e.button.button)
            {
            case SDL_BUTTON_LEFT:
                User->left_click = true;
                break;
            case SDL_BUTTON_RIGHT:
                User->right_click = true;
                break;
            }
        }
        if (e.type == SDL_MOUSEBUTTONUP) {
            switch (e.button.button)
            {
            case SDL_BUTTON_LEFT:
                User->left_click = false;
                break;
            case SDL_BUTTON_RIGHT:
                User->right_click = false;
                break;
            }
        }
    }

    User->getMouseBoard_pos(Board->Dimension, Board->Position);

    if (User->state == chess::boardState::game) {
        User->getTileClicked();
        Board->inputBoard();
    }
    else if (User->state == chess::boardState::promotion) {
        Board->inputBoardPromotion(User);
    }

    SDL_GetMouseState(&User->mouse_pos.x, &User->mouse_pos.y);
}
void window::update() {
    if (User->state == chess::boardState::game)
        Board->updateBoard();
}
void window::render() {
    SDL_SetRenderDrawColor(renderer, 25, 25, 25, 255);
    SDL_RenderClear(renderer);

    Board->drawBoard(renderer, User);
    if (User->state == chess::boardState::promotion)
        Board->drawPromotion(renderer, User);

    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(screen);
}
void window::setup() {
    chess::playerData bot("C:/Users/Kurt/source/repos/Chess/Project2/Project2/stock/lc0/lc0.exe");
    chess::playerData human(User);

    Board->players[0] = bot;
    Board->players[1] = human;

    User->init();
    Board->init(renderer);
}
void window::clean() {}
