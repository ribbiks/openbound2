#pragma once

#include <SDL2/SDL.h>

#include "Vec2.h"

extern SDL_Renderer* renderer;

const SDL_Color PATH_NODE_COL = { 91, 172, 227, 255}; // pal blue 2
const SDL_Color PATH_EDGE_COL = { 97, 104, 182, 255}; // pal blue 3
const SDL_Color WALL_TILE_COL = { 87,  65, 117, 255}; // pal blue 4

const int PF_NODE_RADIUS = 4; // width of pathfinding nodes (for drawing)

struct Line {
    vec2<int> start;
    vec2<int> end;
    SDL_Color color;
};

struct Rect {
    vec2<int> position;  // top-left corner
    vec2<int> size;      // width and height
    SDL_Color color;
};

void draw_line(const Line line) {
    SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
    SDL_RenderDrawLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

void draw_rect(const Rect rect, bool filled = false) {
    SDL_SetRenderDrawColor(renderer, rect.color.r, rect.color.g, rect.color.b, rect.color.a);
    SDL_Rect sdl_rect = {rect.position.x, rect.position.y, rect.size.x, rect.size.y};
    if (filled)
        SDL_RenderFillRect(renderer, &sdl_rect);
    else
        SDL_RenderDrawRect(renderer, &sdl_rect);
}

void draw_background_grid() {
    Line line;
    for (int i = 0; i < RESOLUTION.x; i += GRIDSIZE) {
        line.start = vec2<int>(i, 0);
        line.end = vec2<int>(i, RESOLUTION.y);
        if (i % (GRIDSIZE*2))
            line.color = {22, 22, 22, 255};
        else
            line.color = {36, 36, 36, 255};
        draw_line(line);
    }
    for (int i = 0; i < RESOLUTION.y; i += GRIDSIZE) {
        line.start = vec2<int>(0, i);
        line.end = vec2<int>(RESOLUTION.x, i);
        if (i % (GRIDSIZE*2))
            line.color = {22, 22, 22, 255};
        else
            line.color = {36, 36, 36, 255};
        draw_line(line);
    }
}

void draw_text(std::string text, int x, int y, Font* font) {
    if (font == nullptr) {
        return;
    }
    SDL_Surface* surface = font->render_text(text);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect fps_rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &fps_rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
