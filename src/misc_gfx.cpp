#include "misc_gfx.h"

void draw_line(const Line& line, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

void draw_rect(const Rect& rect, SDL_Color color, bool filled) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect sdl_rect = {rect.position.x, rect.position.y, rect.size.x, rect.size.y};
    if (filled)
        SDL_RenderFillRect(renderer, &sdl_rect);
    else
        SDL_RenderDrawRect(renderer, &sdl_rect);
}

void draw_background_grid(const vec2<int>& offset) {
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

void draw_text(const std::string& text, int x, int y, Font* font) {
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

SDL_Surface* draw_ellipse(int width, int height, SDL_Color color) {
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!surface) {
        return nullptr;
    }
    SDL_Rect floodfill = {0, 0, surface->w, surface->h};
    SDL_FillRect(surface, &floodfill, SDL_MapRGB(surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));
    //
    SDL_LockSurface(surface);
    Uint32* pixels = static_cast<Uint32*>(surface->pixels);
    Uint32 pixel_color = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
    for (int i = 0; i < 360; i++) {
        double angle = i * ANGLE_SCALAR;
        int x = width / 2 + static_cast<int>((width / 2 - 1) * cos(angle));
        int y = height / 2 + static_cast<int>((height / 2 - 1) * sin(angle));
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[y * surface->w + x] = pixel_color;
        }
    }
    SDL_UnlockSurface(surface);
    return surface;
}
