#pragma once
#include <SDL.h>

#include "Vec2.h"

const double TICRATE = 23.8095;
const int MAX_UPDATE_FRAMES = 8;
const double DT = 1.0 / TICRATE;
const double MAX_ACCUM = MAX_UPDATE_FRAMES / TICRATE;

const vec2<int> RESOLUTION = {960, 540};
const int GRIDSIZE = 16;

// how much smaller is the player than a single GRIDSIZE?
const float PLAYER_RADIUS_EPSILON = 0.01f;
const float PLAYER_RADIUS = static_cast<double>(GRIDSIZE) / 2;

const float RADIAN_SCALAR = 57.2957795; // 180 / pi
const float ANGLE_SCALAR  = 0.01745329; // pi / 180

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
const SDL_Color UNIT_HITBOX       = {200, 200, 100, 255};
