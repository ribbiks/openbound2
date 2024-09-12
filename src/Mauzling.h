#pragma once
#include <array>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

const std::array<int,7> MOVE_CYCLE = {2, 8, 9, 5, 6, 7, 2};
const int TURN_SPEED = 40;

class Mauzling {
private:
    vec2<float> position;
    float angle;
    float radius;
    FRect bbox;
    int state;
    SDL_Surface* surface;

public:
    // default constructor
    Mauzling() :
        position(0.0f, 0.0f),
        angle(0.0f),
        radius(PLAYER_RADIUS),
        bbox(FRect()),
        state(0),
        surface(nullptr) {}

    Mauzling(const vec2<float>& pos, std::string image_filename)
        : position(pos), angle(45.0f), radius(PLAYER_RADIUS) {
        
        bbox.position = {position.x - radius, position.y - radius};
        bbox.size = {radius * 2, radius * 2};

        surface = IMG_Load(image_filename.c_str());
        if (surface == nullptr) {
            SDL_Log("Unable to load image %s! SDL_image Error: %s\n", image_filename.c_str(), IMG_GetError());
            return;
        }
    }

    ~Mauzling() {
        if (surface)
            SDL_FreeSurface(surface);
    }

    void update() {
        //
    }

    void draw(const vec2<int> offset) {
        if (surface) {
            // Create a temporary texture from the surface
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture == nullptr) {
                SDL_Log("Unable to create texture! SDL Error: %s\n", SDL_GetError());
                return;
            }
            SDL_Rect dstRect;
            dstRect.x = static_cast<int>(position.x - offset.x - radius);
            dstRect.y = static_cast<int>(position.y - offset.y - radius);
            dstRect.w = static_cast<int>(radius * 2);
            dstRect.h = static_cast<int>(radius * 2);

            SDL_RenderCopyEx(renderer, texture, nullptr, &dstRect, -angle, nullptr, SDL_FLIP_NONE);
            SDL_DestroyTexture(texture);
        }
    }
};
