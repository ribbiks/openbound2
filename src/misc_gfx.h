#pragma once
#include <SDL2/SDL.h>

#include "Font.h"
#include "globals.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

const SDL_Color PATH_NODE_COL = { 91, 172, 227, 255}; // pal blue 2
const SDL_Color PATH_EDGE_COL = { 97, 104, 182, 255}; // pal blue 3
const SDL_Color WALL_TILE_COL = { 87,  65, 117, 255}; // pal blue 4

const SDL_Color GRID_MINOR_COL = {22, 22, 22, 255};
const SDL_Color GRID_MAJOR_COL = {36, 36, 36, 255};

const int PF_NODE_RADIUS = 4; // width of pathfinding nodes (for drawing)

struct Line {
    vec2<int> start;
    vec2<int> end;
};

struct FLine {
    vec2<float> start;
    vec2<float> end;
};

struct Rect {
    vec2<int> position;  // top-left corner
    vec2<int> size;      // width and height
};

struct FRect {
    vec2<float> position;  // top-left corner
    vec2<float> size;      // width and height
};

void draw_line(const Line line, SDL_Color color);
void draw_rect(const Rect rect, SDL_Color color, bool filled = false);
void draw_background_grid(const vec2<int> offset);
void draw_text(std::string text, int x, int y, Font* font);
