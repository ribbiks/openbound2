#include "inputs.h"

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
                case SDLK_SPACE:
                    inputs->key_space = true;
                    break;
                case SDLK_RETURN:
                    inputs->key_enter = true;
                    break;
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    inputs->key_shift = true;
                    break;
                case SDLK_ESCAPE:
                    inputs->key_escape = true;
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
                case SDLK_SPACE:
                    inputs->key_space = false;
                    break;
                case SDLK_RETURN:
                    inputs->key_enter = false;
                    break;
                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    inputs->key_shift = false;
                    break;
                case SDLK_ESCAPE:
                    inputs->key_escape = false;
                    break;
            }
        }
    }
}
