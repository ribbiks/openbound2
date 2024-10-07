#pragma once
#include <SDL.h>
#include <string>
#include <vector>

#include "Font.h"
#include "geometry.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

const int PF_NODE_RADIUS = 4; // width of pathfinding nodes (for drawing)

SDL_Surface* load_image(const std::string& image_filename, bool colorkey = true);
std::vector<SDL_Surface*> load_spritesheet(const std::string& image_filename, const vec2<int>& sprite_dimensions);
SDL_Surface* rescale_surface(SDL_Surface* src, int new_width, int new_height);
void draw_line(const Line& line, SDL_Color color);
void draw_thick_line(SDL_Renderer* renderer, const Line& line, SDL_Color color, int thickness);
void draw_rect(const Rect& rect, SDL_Color color, bool filled = false);
void draw_rect(const SDL_Rect& sdl_rect, SDL_Color color, bool filled = false);
void draw_thick_rect(const Rect& rect, SDL_Color color, int thickness);
void draw_background_grid(const vec2<int>& offset);
void draw_text(const std::string& text, int x, int y, Font* font);
SDL_Surface* draw_ellipse(int width, int height, SDL_Color color);
