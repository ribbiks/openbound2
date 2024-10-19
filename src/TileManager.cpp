#include "TileManager.h"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"

using json = nlohmann::json;

TileManager::TileManager(const std::string& tiledata_json) : all_tile_data() {
    // load map from json
    std::ifstream input_file(tiledata_json);
    json loaded_data = json::parse(input_file);
    //
    std::string tile_image_dir = loaded_data["tile_image_dir"];
    for (const auto& item : loaded_data["tile_data"]) {
        if (item.size() != 5)
            throw std::invalid_argument("Invalid tile json");
        int tile_index = item[0].get<int>();
        int tile_is_blocking = item[1].get<int>();
        std::string tile_image_filename = tile_image_dir + item[2].get<std::string>();
        bool tile_is_wall = false;
        if (tile_is_blocking > 0)
            tile_is_wall = true;
        std::vector<int> animated_tile_iscript = item[3].get<std::vector<int>>();
        std::vector<float> scroll_params = item[4].get<std::vector<float>>();
        vec2<float> scroll_xy = {0.0f, 0.0f};
        if (scroll_params.size() == 2) {
            float scroll_mag = value_clamp(scroll_params[0], 0.0f, MAX_SCROLL_MAGNITUDE);
            float scroll_x = scroll_mag * std::cos(ANGLE_SCALAR * scroll_params[1]);
            float scroll_y = scroll_mag * std::sin(ANGLE_SCALAR * scroll_params[1]);
            scroll_xy = {scroll_x, scroll_y};
        }
        //
        // static tiles
        //
        if (animated_tile_iscript.size() == 0) {
            SDL_Surface* temp_surface = load_image(tile_image_filename);
            all_tile_data.push_back({SDL_CreateTextureFromSurface(renderer, temp_surface), tile_is_wall, false, scroll_xy});
            SDL_FreeSurface(temp_surface);
        }
        //
        // animated tiles
        //
        else {
            std::string animated_tile_name = std::to_string(tile_index);
            animation_manager.add_animation(animated_tile_name, tile_image_filename, {GRIDSIZE,GRIDSIZE}, animated_tile_iscript);
            animation_manager.start_new_animation(animated_tile_name, animated_tile_name, {0,0}, true);
            all_tile_data.push_back({nullptr, tile_is_wall, true, scroll_xy});
        }
    }
};

TileManager::~TileManager() {
    for(size_t i = 0; i < all_tile_data.size(); ++i)
        SDL_DestroyTexture(all_tile_data[i].texture);
}

int TileManager::get_number_of_loaded_tiles() {
    return all_tile_data.size();
}

bool TileManager::get_tile_iswall(size_t i) {
    if (i >= all_tile_data.size())
        throw std::invalid_argument("Requesting invalid tile");
    return all_tile_data[i].is_wall;
}

vec2<float> TileManager::get_tile_scroll(size_t i) {
    if (i >= all_tile_data.size())
        throw std::invalid_argument("Requesting invalid tile");
    return all_tile_data[i].scroll_parameters;
}

SDL_Texture* TileManager::get_tile_texture(size_t i) {
    if (i >= all_tile_data.size())
        throw std::invalid_argument("Requesting invalid tile");
    // static tiles
    if (!all_tile_data[i].is_animated)
        return all_tile_data[i].texture;
    // animated tiles
    std::string animated_tile_name = std::to_string(i);
    return animation_manager.get_animating_texture(animated_tile_name);
}

void TileManager::tick() {
    animation_manager.tick();
}
