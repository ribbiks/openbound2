#pragma once

#include <SDL.h>

extern SDL_Window* window;

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
    bool key_enter;
    bool key_space;
    bool key_shift;
    bool key_escape;
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
        key_enter(other.key_enter),
        key_space(other.key_space),
        key_shift(other.key_shift),
        key_escape(other.key_escape),
        quit(other.quit) {}
};

void get_inputs(player_inputs* inputs);
