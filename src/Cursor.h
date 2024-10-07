#pragma once
#include <string>
#include <vector>

#include <SDL.h>

#include "Vec2.h"

extern SDL_Renderer* renderer;

static const std::vector<vec2<int>> CLICK_SCALARS = {{25,17}, {29,19}, {31,20}, {29,19}, {27,18}};
static const int animation_speed_cursor = 3;

class Cursor {
private:
    vec2<int> position;
    int frame_cursor;
    int frame_click;
    int animation_tick_cursor;
    int animation_tick_click_animation;
    std::vector<vec2<int>> cursor_sizes;
    std::vector<SDL_Texture*> textures_cursor;
    std::vector<SDL_Texture*> textures_click;
    std::vector<SDL_Texture*> textures_shiftclick;
    vec2<int> click_animation_position;
    bool click_animation_is_queue;

public:
    Cursor(std::vector<std::string> cursor_images, std::string click_image, std::string shiftclick_image);
    ~Cursor();
    vec2<int> get_pos();
    void start_click_animation(bool is_queue);
    void update(const vec2<int>& new_position);
    void tick();
    void draw();
};
