#pragma once
#include <string>
#include <vector>

#include <SDL.h>

#include "globals.h"
#include "misc_gfx.h"

extern SDL_Renderer* renderer;

static const std::vector<vec2<int>> CLICK_SCALARS = {{25,17}, {27,18}, {29,19}, {27,18}};

class Cursor {
private:
    vec2<int> position;
    int frame_cursor;
    int frame_click;
    int animation_speed_cursor = 3;
    int animation_tick_cursor = 0;
    std::vector<SDL_Surface*> surfaces_cursor;
    std::vector<SDL_Surface*> surfaces_click;
    std::vector<SDL_Surface*> surfaces_shiftclick;

public:
    Cursor(std::vector<std::string> cursor_images, std::string click_image, std::string shiftclick_image) :
        position(vec2<int>()),
        frame_cursor(0),
        frame_click(-1) {

        for (size_t i = 0; i < cursor_images.size(); ++i) {
            surfaces_cursor.push_back(load_image(cursor_images[i]));
        }

        SDL_Surface* temp_surface1 = load_image(click_image);
        SDL_Surface* temp_surface2 = load_image(shiftclick_image);
        for (size_t i = 0; i < CLICK_SCALARS.size(); ++i) {
            surfaces_click.push_back(rescale_surface(temp_surface1, CLICK_SCALARS[i].x, CLICK_SCALARS[i].y));
            surfaces_shiftclick.push_back(rescale_surface(temp_surface2, CLICK_SCALARS[i].x, CLICK_SCALARS[i].y));
        }
        SDL_FreeSurface(temp_surface1);
        SDL_FreeSurface(temp_surface2);
    }

    ~Cursor() {
        for (size_t i = 0; i < surfaces_cursor.size(); ++i)
            if (surfaces_cursor[i])
                SDL_FreeSurface(surfaces_cursor[i]);
        for (size_t i = 0; i < surfaces_click.size(); ++i)
            if (surfaces_click[i])
                SDL_FreeSurface(surfaces_click[i]);
    }

    vec2<int> get_pos() {
        return position;
    }

    void update(const vec2<int>& new_position) {
        position = new_position;
    }

    void tick() {
        animation_tick_cursor += 1;
        if (animation_tick_cursor >= animation_speed_cursor) {
            frame_cursor = (frame_cursor + 1) % surfaces_cursor.size();
            animation_tick_cursor = 0;
        }
    }

    void draw() {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surfaces_cursor[frame_cursor]);
        if (texture == nullptr) {
            SDL_Log("Unable to create texture! SDL Error: %s\n", SDL_GetError());
            return;
        }
        SDL_Rect rect = {position.x - 1, position.y - 1, surfaces_cursor[frame_cursor]->w, surfaces_cursor[frame_cursor]->h};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }
};
