#pragma once
#include <string>
#include <vector>

#include <SDL.h>

#include "Array2D.h"
#include "Obstacle.h"
#include "pathfinding.h"
#include "TileManager.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

static const int PF_NODE_RADIUS = 4; // width of pathfinding nodes (for drawing)

class WorldMap {
private:
    PathfindingData pf_data;
    Array2D<int> tile_dat;
    Array2D<bool> wall_dat;
    vec2<int> player_start;
    std::string map_name;
    std::vector<Obstacle> obstacles;
    int currently_active_ob = -1;
    TileManager* tile_manager = nullptr;

public:
    WorldMap(const std::string& map_filename);
    ~WorldMap();
    vec2<int> get_map_size();
    vec2<int> get_start_pos();
    vec2<float> get_move_pos(const vec2<float>& position, const vec2<float>& goal_position);
    vec2<float> get_scrolled_pos(const vec2<float>& position);
    void change_map_tiles(const std::vector<vec2<int>>& coord_list, const std::vector<int>& tileid_list);
    void set_current_obstacle(int obnum);
    std::vector<vec2<int>> pathfind(const vec2<int>& start_pos, const vec2<int>& end_pos);
    std::vector<Event> tick();
    void draw(const vec2<int>& offset);
};
