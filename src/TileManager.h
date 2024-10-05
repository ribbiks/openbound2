#pragma once
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <SDL.h>

#include "misc_gfx.h"

using json = nlohmann::json;

struct tile_dat {
    SDL_Surface* surface;
    bool is_wall;
};

class TileManager {
private:
     std::vector<tile_dat> all_tile_data;

public:
    TileManager(const std::string& tiledata_json) : all_tile_data() {
        // load map from json
        std::ifstream input_file(tiledata_json);
        json loaded_data = json::parse(input_file);
        //
        std::string tile_image_dir = loaded_data["tile_image_dir"];
        for (const auto& item : loaded_data["tile_data"]) {
            //int tile_index = item[0].get<int>();
            int tile_is_blocking = item[1].get<int>();
            std::string tile_image_filename = item[2].get<std::string>();
            bool tile_is_wall = false;
            if (tile_is_blocking > 0)
                tile_is_wall = true;
            all_tile_data.push_back({load_image(tile_image_dir + tile_image_filename), tile_is_wall});
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
        return all_tile_data[i].surface;
    }
};
