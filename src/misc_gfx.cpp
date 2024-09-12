#include "misc_gfx.h"

void draw_line(const Line line, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

void draw_rect(const Rect rect, SDL_Color color, bool filled) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect sdl_rect = {rect.position.x, rect.position.y, rect.size.x, rect.size.y};
    if (filled)
        SDL_RenderFillRect(renderer, &sdl_rect);
    else
        SDL_RenderDrawRect(renderer, &sdl_rect);
}

void draw_background_grid(const vec2<int> offset) {
    Line line;
    for (int i = 0; i < RESOLUTION.x; i += GRIDSIZE) {
        line.start = vec2<int>(i - (offset.x % GRIDSIZE), 0);
        line.end = vec2<int>(i - (offset.x % GRIDSIZE), RESOLUTION.y);
        if (i % (GRIDSIZE*2))
            draw_line(line, GRID_MINOR_COL);
        else
            draw_line(line, GRID_MAJOR_COL);
    }
    for (int i = 0; i < RESOLUTION.y; i += GRIDSIZE) {
        line.start = vec2<int>(0, i - (offset.y % GRIDSIZE));
        line.end = vec2<int>(RESOLUTION.x, i - (offset.y % GRIDSIZE));
        if (i % (GRIDSIZE*2))
            draw_line(line, GRID_MINOR_COL);
        else
            draw_line(line, GRID_MAJOR_COL);
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
