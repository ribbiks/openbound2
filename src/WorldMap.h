#pragma once
#include <string>
#include <SDL.h>

#include "Array2D.h"
#include "pathfinding.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

class WorldMap {
private:
    PathfindingData pf_data;
    Array2D<bool> wall_dat;
    vec2<int> player_start;
    std::string map_name;

public:
    WorldMap(const std::string& map_filename);
    vec2<int> get_map_size();
    vec2<int> get_start_pos();
    void tick();
    void draw(const vec2<int>& offset);
};
