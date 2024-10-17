#pragma once
#include <string>
#include <vector>

#include <SDL.h>

#include "AnimationManager.h"

extern SDL_Renderer* renderer;

struct TileMetadata {
    SDL_Texture* texture;
    bool is_wall;
    bool is_animated;
    vec2<float> scroll_parameters; // (magnitude, angle)
};

class TileManager {
private:
     std::vector<TileMetadata> all_tile_data;
     AnimationManager animation_manager;

public:
    TileManager(const std::string& tiledata_json);
    ~TileManager();
    int get_number_of_loaded_tiles();
    bool get_tile_iswall(size_t i);
    vec2<float> get_tile_scroll(size_t i);
    SDL_Texture* get_tile_texture(size_t i);
    void tick();
};
