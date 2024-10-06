#pragma once
#include <string>
#include <SDL.h>

#include "Array2D.h"
#include "pathfinding.h"
#include "TileManager.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

class WorldMap {
private:
    PathfindingData pf_data;
    Array2D<int> tile_dat;
    Array2D<bool> wall_dat;
    vec2<int> player_start;
    std::string map_name;
    TileManager* tile_manager = nullptr;

public:
    WorldMap(const std::string& map_filename);
    ~WorldMap();
    vec2<int> get_map_size();
    vec2<int> get_start_pos();
    vec2<float> get_scrolled_pos(const vec2<float>& position);
    std::vector<vec2<int>> pathfind(const vec2<int>& start_pos, const vec2<int>& end_pos);
    void tick();
    void draw(const vec2<int>& offset);
};
