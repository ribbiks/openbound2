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

    void tick() {
        //
    }

    void draw() {
        //
    }
};
