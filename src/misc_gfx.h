#pragma once
#include <cmath>
#include <SDL2/SDL.h>

#include "Font.h"
#include "geometry.h"
#include "globals.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

const int PF_NODE_RADIUS = 4; // width of pathfinding nodes (for drawing)

void draw_line(const Line& line, SDL_Color color);
void draw_rect(const Rect& rect, SDL_Color color, bool filled = false);
void draw_background_grid(const vec2<int>& offset);
void draw_text(const std::string& text, int x, int y, Font* font);
SDL_Surface* draw_ellipse(int width, int height, SDL_Color color);
