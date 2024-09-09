#pragma once

#include <SDL2/SDL.h>

struct player_inputs {
    int mouse_x;
    int mouse_y;
    Uint32 mouse_state;
    bool leftmouse_down;
    bool leftmouse_up;
    bool midmouse_down;
    bool midmouse_up;
    bool rightmouse_down;
    bool rightmouse_up;
    bool move_up;
    bool move_down;
    bool move_left;
    bool move_right;
    bool quit;

    // default constructor
    player_inputs() {}

    // copy constructor
    player_inputs(const player_inputs& other) : 
        mouse_x(other.mouse_x),
        mouse_y(other.mouse_y),
        mouse_state(other.mouse_state),
        leftmouse_down(other.leftmouse_down),
        leftmouse_up(other.leftmouse_up),
        midmouse_down(other.midmouse_down),
        midmouse_up(other.midmouse_up),
        rightmouse_down(other.rightmouse_down),
        rightmouse_up(other.rightmouse_up),
        move_up(other.move_up),
        move_down(other.move_down),
        move_left(other.move_left),
        move_right(other.move_right),
        quit(other.quit) {}
};

void get_inputs(player_inputs* inputs) {
    // mouse pos
    inputs->mouse_state = SDL_GetMouseState(&inputs->mouse_x, &inputs->mouse_y);
    // button presses
    SDL_Event e;
    inputs->leftmouse_down = false;
    inputs->midmouse_down = false;
    inputs->rightmouse_down = false;
    inputs->leftmouse_up = false;
    inputs->midmouse_up = false;
    inputs->rightmouse_up = false;
    inputs->quit = false;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            inputs->quit = true;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            switch (e.button.button) {
                case SDL_BUTTON_LEFT:
                    inputs->leftmouse_down = true;
                    break;
                case SDL_BUTTON_MIDDLE:
                    inputs->midmouse_down = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    inputs->rightmouse_down = true;
                    break;
            }
        }
        else if (e.type == SDL_MOUSEBUTTONUP) {
            switch (e.button.button) {
                case SDL_BUTTON_LEFT:
                    inputs->leftmouse_up = true;
                    break;
                case SDL_BUTTON_MIDDLE:
                    inputs->midmouse_up = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    inputs->rightmouse_up = true;
                    break;
            }
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    inputs->move_up = true;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    inputs->move_down = true;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    inputs->move_left = true;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    inputs->move_right = true;
                    break;
            }
        }
        else if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    inputs->move_up = false;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    inputs->move_down = false;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    inputs->move_left = false;
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    inputs->move_right = false;
                    break;
            }
        }
    }
}