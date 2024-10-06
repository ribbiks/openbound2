#pragma once
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <SDL.h>

#include "AnimationManager.h"
#include "misc_gfx.h"

using json = nlohmann::json;

struct TileMetadata {
    SDL_Surface* surface;
    bool is_wall;
    bool is_animated;
};

class TileManager {
private:
     std::vector<TileMetadata> all_tile_data;
     AnimationManager animation_manager;

public:
    TileManager(const std::string& tiledata_json) : all_tile_data() {
        // load map from json
        std::ifstream input_file(tiledata_json);
        json loaded_data = json::parse(input_file);
        //
        std::string tile_image_dir = loaded_data["tile_image_dir"];
        for (const auto& item : loaded_data["tile_data"]) {
            int tile_index = item[0].get<int>();
            int tile_is_blocking = item[1].get<int>();
            std::string tile_image_filename = tile_image_dir + item[2].get<std::string>();
            bool tile_is_wall = false;
            if (tile_is_blocking > 0)
                tile_is_wall = true;
            //
            // static tiles
            //
            if (item.size() == 3)
                all_tile_data.push_back({load_image(tile_image_filename), tile_is_wall, false});
            //
            // animated tiles
            //
            else if (item.size() == 4) {
                std::vector<int> animated_tile_iscript = item[3].get<std::vector<int>>();
                std::string animated_tile_name = std::to_string(tile_index);
                animation_manager.add_animation(animated_tile_name, tile_image_filename, {GRIDSIZE,GRIDSIZE}, animated_tile_iscript);
                animation_manager.start_new_animation(animated_tile_name, animated_tile_name, {0,0}, true);
                all_tile_data.push_back({nullptr, tile_is_wall, true});
            }
            else
                throw std::invalid_argument("Invalid tile json (wrong tuple size)");
        }
    };

    ~TileManager() {
        for(size_t i = 0; i < all_tile_data.size(); ++i)
            SDL_FreeSurface(all_tile_data[i].surface);
    }

    int get_number_of_loaded_tiles() {
        return all_tile_data.size();
    }

    bool get_tile_iswall(int i) {
        int num_elements = all_tile_data.size();
        if (i < 0 or i >= num_elements)
            throw std::invalid_argument("Requesting invalid tile");
        return all_tile_data[i].is_wall;
    }

    SDL_Surface* get_tile_surface(int i) {
        int num_elements = all_tile_data.size();
        if (i < 0 or i >= num_elements)
            throw std::invalid_argument("Requesting invalid tile");
        // static tiles
        if (!all_tile_data[i].is_animated)
            return all_tile_data[i].surface;
        // animated tiles
        return animation_manager.get_animating_surface(std::to_string(i));
    }

    void tick() {
        animation_manager.tick();
    }
};
