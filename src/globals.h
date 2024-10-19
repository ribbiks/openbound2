#pragma once
#include <string>

#include <SDL.h>

#include "Vec2.h"

// general purpose struct for event messages
struct Event {
    std::string type;
    std::string data_str;
    std::string data_str_2;
    vec2<int> data_vec;
};

const double TICRATE = 23.8095;
const int MAX_UPDATE_FRAMES = 8;
const double DT = 1.0 / TICRATE;
const double MAX_ACCUM = MAX_UPDATE_FRAMES / TICRATE;

const vec2<int> RESOLUTION = {960, 540};
const int GRIDSIZE = 16;
const float F_GRIDSIZE = static_cast<float>(GRIDSIZE);

const float EPSILON = 0.0001f;

const float PLAYER_RADIUS = 8.0f;
const float PLAYER_RADIUS_GRIDUNITS = PLAYER_RADIUS / F_GRIDSIZE;

const float RADIAN_SCALAR = 57.2957795; // 180 / pi
const float ANGLE_SCALAR  = 0.01745329; // pi / 180

const float MAX_SCROLL_MAGNITUDE = static_cast<float>(GRIDSIZE);

const SDL_Color FONT_LETTER_COL = {  0,   0,   0, 255};
const SDL_Color FONT_DELIM_COL  = {127, 127, 127, 255};
const SDL_Color TRANS_COL       = {255,   0, 255, 255};

const SDL_Color PATH_NODE_COL = { 91, 172, 227, 255}; // pal blue 2
const SDL_Color PATH_EDGE_COL = { 97, 104, 182, 255}; // pal blue 3
const SDL_Color WALL_TILE_COL = { 87,  65, 117, 255}; // pal blue 4

const SDL_Color GRID_MINOR_COL = {22, 22, 22, 255};
const SDL_Color GRID_MAJOR_COL = {36, 36, 36, 255};

const SDL_Color SELECTION_BOX_COL = { 50, 200,  50, 255};
const SDL_Color UNIT_ELLIPSE_COL  = { 50, 200,  50, 255};
const SDL_Color UNIT_HITBOX_COL   = {200, 200, 100, 255};

const SDL_Color OB_STARTBOX_COL = {100, 250, 100, 127};
const SDL_Color OB_ENDBOX_COL   = {250, 100, 100, 127};
const SDL_Color OB_REVIVE_COL   = {220, 220, 220, 255};
const SDL_Color OB_LOC_COL      = {150, 150, 150, 127};
