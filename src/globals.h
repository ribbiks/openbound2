#pragma once

#include "Vec2.h"

const int TICRATE = 4;
const int MAX_UPDATE_FRAMES = 8;
const double DT = 1.0 / TICRATE;
const double MAX_ACCUM = MAX_UPDATE_FRAMES / static_cast<double>(TICRATE);

const vec2<int> RESOLUTION = {480, 270};
const int GRIDSIZE = 16;

// how much smaller is the player than a single GRIDSIZE?
const float PLAYER_RADIUS_EPSILON = 0.01f;
const float PLAYER_RADIUS = static_cast<double>(GRIDSIZE) / 2;
