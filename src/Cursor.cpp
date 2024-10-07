#include "Cursor.h"

#include "misc_gfx.h"

Cursor::Cursor(std::vector<std::string> cursor_images, std::string click_image, std::string shiftclick_image) :
    position(vec2<int>()),
    frame_cursor(0),
    frame_click(-1),
    animation_tick_cursor(0),
    animation_tick_click_animation(-1) {

    for (size_t i = 0; i < cursor_images.size(); ++i) {
        SDL_Surface* temp_surface = load_image(cursor_images[i]);
        textures_cursor.push_back(SDL_CreateTextureFromSurface(renderer, temp_surface));
        cursor_sizes.push_back({temp_surface->w, temp_surface->h});
        SDL_FreeSurface(temp_surface);
    }

    SDL_Surface* temp_surface1 = load_image(click_image);
    SDL_Surface* temp_surface2 = load_image(shiftclick_image);
    for (size_t i = 0; i < CLICK_SCALARS.size(); ++i) {
        SDL_Surface* temp_surface3 = rescale_surface(temp_surface1, CLICK_SCALARS[i].x, CLICK_SCALARS[i].y);
        SDL_Surface* temp_surface4 = rescale_surface(temp_surface2, CLICK_SCALARS[i].x, CLICK_SCALARS[i].y);
        textures_click.push_back(SDL_CreateTextureFromSurface(renderer, temp_surface3));
        textures_shiftclick.push_back(SDL_CreateTextureFromSurface(renderer, temp_surface4));
        SDL_FreeSurface(temp_surface3);
        SDL_FreeSurface(temp_surface4);
    }
    SDL_FreeSurface(temp_surface1);
    SDL_FreeSurface(temp_surface2);
}

Cursor::~Cursor() {
    for (size_t i = 0; i < textures_cursor.size(); ++i)
        if (textures_cursor[i])
            SDL_DestroyTexture(textures_cursor[i]);
    for (size_t i = 0; i < textures_click.size(); ++i)
        if (textures_click[i])
            SDL_DestroyTexture(textures_click[i]);
    for (size_t i = 0; i < textures_shiftclick.size(); ++i)
        if (textures_shiftclick[i])
            SDL_DestroyTexture(textures_shiftclick[i]);
}

vec2<int> Cursor::get_pos() {
    return position;
}

void Cursor::start_click_animation(bool is_queue) {
    click_animation_position = position;
    click_animation_is_queue = is_queue;
    animation_tick_click_animation = 0;
}

void Cursor::update(const vec2<int>& new_position) {
    position = new_position;
}

void Cursor::tick() {
    animation_tick_cursor += 1;
    if (animation_tick_cursor >= animation_speed_cursor) {
        frame_cursor = (frame_cursor + 1) % textures_cursor.size();
        animation_tick_cursor = 0;
    }
    if (animation_tick_click_animation >= 0) {
        animation_tick_click_animation += 1;
        if (animation_tick_click_animation >= static_cast<int>(textures_click.size()))
            animation_tick_click_animation = -1;
    }
}

void Cursor::draw() {
    SDL_Rect rect = {position.x - 1, position.y - 1, cursor_sizes[frame_cursor].x, cursor_sizes[frame_cursor].y};
    SDL_RenderCopy(renderer, textures_cursor[frame_cursor], nullptr, &rect);
    //
    if (animation_tick_click_animation >= 1) {
        int surface_w = CLICK_SCALARS[animation_tick_click_animation - 1].x;
        int surface_h = CLICK_SCALARS[animation_tick_click_animation - 1].y;
        rect = {click_animation_position.x - surface_w/2, click_animation_position.y - surface_h/2, surface_w, surface_h};
        if (click_animation_is_queue)
            SDL_RenderCopy(renderer, textures_shiftclick[animation_tick_click_animation - 1], nullptr, &rect);
        else
            SDL_RenderCopy(renderer, textures_click[animation_tick_click_animation - 1], nullptr, &rect);
    }
}
