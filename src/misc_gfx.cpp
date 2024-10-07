#include <cmath>
#include <stdexcept>

#include <SDL.h>
#include <SDL_image.h>

#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"

SDL_Surface* load_image(const std::string& image_filename, bool colorkey) {
    SDL_Surface* surface = IMG_Load(image_filename.c_str());
    if (surface == nullptr) {
        SDL_Log("Unable to load image %s! SDL_image Error: %s\n", image_filename.c_str(), IMG_GetError());
        return nullptr;
    }
    if (colorkey)
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));
    return surface;
}

std::vector<SDL_Surface*> load_spritesheet(const std::string& image_filename, const vec2<int>& sprite_dimensions) {
    SDL_Surface* full_img = load_image(image_filename);
    if (full_img->w % sprite_dimensions.x > 0 || full_img->h % sprite_dimensions.y > 0)
        throw std::invalid_argument("invalid spritesheet size");
    int num_cols = full_img->w / sprite_dimensions.x;
    int num_rows = full_img->h / sprite_dimensions.y;

    std::vector<SDL_Surface*> out_vector;
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            SDL_Rect sprite_rect = {j * sprite_dimensions.x, i * sprite_dimensions.y, sprite_dimensions.x, sprite_dimensions.y};
            SDL_Surface* sprite_surface = SDL_CreateRGBSurface(0, sprite_rect.w, sprite_rect.h, 32, 0, 0, 0, 0);
            SDL_Rect floodfill = {0, 0, sprite_surface->w, sprite_surface->h};
            SDL_FillRect(sprite_surface, &floodfill, SDL_MapRGB(sprite_surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));
            SDL_BlitSurface(full_img, &sprite_rect, sprite_surface, nullptr);
            SDL_SetColorKey(sprite_surface, SDL_TRUE, SDL_MapRGB(sprite_surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));
            out_vector.push_back(sprite_surface);
        }
    }
    SDL_FreeSurface(full_img);

    return out_vector;
}

SDL_Surface* rescale_surface(SDL_Surface* src, int new_width, int new_height) {
    if (!src)
        return nullptr;
    SDL_Surface* dest = SDL_CreateRGBSurface(0, new_width, new_height, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
    if (!dest)
        return nullptr;
    SDL_Rect src_rect = {0, 0, src->w, src->h};
    SDL_Rect dst_rect = {0, 0, new_width, new_height};
    if (SDL_BlitScaled(src, &src_rect, dest, &dst_rect) != 0) {
        SDL_FreeSurface(dest);
        return nullptr;
    }
    return dest;
}

void draw_line(const Line& line, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

void draw_thick_line(const Line& line, SDL_Color color, int thickness) {
    int dx = line.end.x - line.start.x;
    int dy = line.end.y - line.start.y;
    double length = std::sqrt(dx * dx + dy * dy);
    if (length < EPSILON)
        return;

    double unitX = dx / length;
    double unitY = dy / length;
    double perpX = -unitY;
    double perpY = unitX;

    std::vector<SDL_Point> points;
    for (int i = -thickness / 2; i < (thickness + 1) / 2; ++i) {
        int offsetX = static_cast<int>(std::round(i * perpX));
        int offsetY = static_cast<int>(std::round(i * perpY));
        SDL_Point start_point = {line.start.x + offsetX, line.start.y + offsetY};
        SDL_Point end_point = {line.end.x + offsetX, line.end.y + offsetY};
        points.push_back(start_point);
        points.push_back(end_point);
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLines(renderer, points.data(), points.size());
}

void draw_rect(const Rect& rect, SDL_Color color, bool filled) {
    draw_rect({rect.position.x, rect.position.y, rect.size.x, rect.size.y}, color, filled);
}

void draw_rect(const SDL_Rect& sdl_rect, SDL_Color color, bool filled) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (filled)
        SDL_RenderFillRect(renderer, &sdl_rect);
    else
        SDL_RenderDrawRect(renderer, &sdl_rect);
}

void draw_thick_rect(const Rect& rect, SDL_Color color, int thickness) {
    Line line1 = {rect.position,
                  rect.position + vec2<int>(rect.size.x, 0)};
    Line line2 = {rect.position + vec2<int>(rect.size.x, 0),
                  rect.position + rect.size};
    Line line3 = {rect.position + rect.size,
                  rect.position + vec2<int>(0, rect.size.y)};
    Line line4 = {rect.position + vec2<int>(0, rect.size.y),
                  rect.position};
    draw_thick_line(line1, color, thickness);
    draw_thick_line(line2, color, thickness);
    draw_thick_line(line3, color, thickness);
    draw_thick_line(line4, color, thickness);
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
