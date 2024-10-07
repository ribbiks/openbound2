#include "WorldMap.h"

#include <fstream>
#include <stdexcept>
#include <vector>

#include <nlohmann/json.hpp>

#include "Array2D.h"
#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"
#include "pathfinding.h"
#include "Vec2.h"

using json = nlohmann::json;

WorldMap::WorldMap(const std::string& map_filename) {
    // load map from json
    std::ifstream input_file(map_filename);
    json loaded_data = json::parse(input_file);

    map_name = loaded_data["map_name"];
    int map_width = loaded_data["map_width"];
    int map_height = loaded_data["map_height"];
    if (map_width < 1 or map_height < 1)
        throw std::invalid_argument("Map has invalid dimensions");

    std::string tileset_json = loaded_data["tileset"];
    tile_manager = new TileManager(tileset_json);
    std::vector<int> map_tile_vector = loaded_data["tile_dat"].get<std::vector<int>>();
    int max_tile_index = *std::max_element(map_tile_vector.begin(), map_tile_vector.end());
    if (max_tile_index >= tile_manager->get_number_of_loaded_tiles())
        throw std::invalid_argument("Map has invalid tiles");

    tile_dat = Array2D<int>(map_width, map_height, map_tile_vector);
    wall_dat = Array2D<bool>(tile_dat.width(), tile_dat.height(), false);
    for (int i = 0; i < tile_dat.width(); ++i) {
        for (int j = 0; j < tile_dat.height(); ++j) {
            if (tile_manager->get_tile_iswall(tile_dat[i][j]))
                wall_dat[i][j] = true;
        }
    }
    std::vector<int> start_pos = loaded_data["start_pos"].get<std::vector<int>>();
    if (start_pos.size() != 2 || start_pos[0] < 0 || start_pos[1] < 0)
        throw std::invalid_argument("Map has invalid start_pos");
    player_start = {start_pos[0], start_pos[1]};

    printf("map_name: %s (%ix%i)\n", map_name.c_str(), tile_dat.width(), tile_dat.height());
    printf("player_start: (%i,%i)\n", player_start.x, player_start.y);

    double start_time = SDL_GetTicks() / 1000.0;
    pf_data = get_pathfinding_data(wall_dat);
    double end_time = SDL_GetTicks() / 1000.0 - start_time;
    printf("map processed in %f seconds\n", end_time);
}

WorldMap::~WorldMap() {
    delete tile_manager;
    tile_manager = nullptr;
}

vec2<int> WorldMap::get_map_size() {
    return vec2<int>(GRIDSIZE * wall_dat.width(), GRIDSIZE * wall_dat.height());
}

vec2<int> WorldMap::get_start_pos() {
    return GRIDSIZE * player_start + vec2<int>({GRIDSIZE/2, GRIDSIZE/2});
}

vec2<float> WorldMap::get_scrolled_pos(const vec2<float>& position) {
    vec2<float> map_position = {position.x / GRIDSIZE, position.y / GRIDSIZE};
    vec2<float> scroll_xy = tile_manager->get_tile_scroll(tile_dat[static_cast<int>(map_position.x)][static_cast<int>(map_position.y)]);
    // deal with floating point
    if (std::abs(scroll_xy.x) < EPSILON)
        scroll_xy.x = 0.0f;
    if (std::abs(scroll_xy.y) < EPSILON)
        scroll_xy.y = 0.0f;
    if (std::abs(scroll_xy.x) > EPSILON || std::abs(scroll_xy.y) > EPSILON) {
        // check if we have clearance to nudged position
        vec2<float> goal_pos = position + scroll_xy;
        vec2<float> goal_pos_map = {goal_pos.x / GRIDSIZE, goal_pos.y / GRIDSIZE};
        if (line_of_sight_unit(map_position, goal_pos_map, wall_dat))
            return goal_pos;
        // scroll pushes us against a wall
        // TODO
    }
    return position;
}

std::vector<vec2<int>> WorldMap::pathfind(const vec2<int>& start_pos, const vec2<int>& end_pos) {
    vec2<int> map_size = get_map_size() - vec2<int>(1,1);
    vec2<int> start_pos_bounded = {value_clamp(start_pos.x, 0, map_size.x), value_clamp(start_pos.y, 0, map_size.y)};
    vec2<int> end_pos_bounded = {value_clamp(end_pos.x, 0, map_size.x), value_clamp(end_pos.y, 0, map_size.y)};
    return get_pathfinding_waypoints(start_pos_bounded, end_pos_bounded, pf_data, wall_dat);
}

void WorldMap::tick() {
    tile_manager->tick();
}

void WorldMap::draw(const vec2<int>& offset) {
    // draw terrain
    int start_x = offset.x / GRIDSIZE;
    int start_y = offset.y / GRIDSIZE;
    int end_x = (offset.x + RESOLUTION.x) / GRIDSIZE + 1;
    int end_y = (offset.y + RESOLUTION.y) / GRIDSIZE + 1;
    for (int i = start_x; i < end_x && i < tile_dat.width(); ++i) {
        for (int j = start_y; j < end_y && j < tile_dat.height(); ++j) {
            SDL_Rect rect = {GRIDSIZE * i - offset.x, GRIDSIZE * j - offset.y, GRIDSIZE, GRIDSIZE};
            SDL_RenderCopy(renderer, tile_manager->get_tile_texture(tile_dat[i][j]), nullptr, &rect);
        }
    }

    //// draw impassable tiles
    //for (int i = 0; i < wall_dat.width(); ++i) {
    //    for (int j = 0; j < wall_dat.height(); ++j) {
    //        if (wall_dat[i][j]) {
    //            Rect rect = {GRIDSIZE * vec2<int>(i,j), {GRIDSIZE, GRIDSIZE}};
    //            rect.position -= offset;
    //            draw_rect(rect, WALL_TILE_COL, true);
    //        }
    //    }
    //}

    // draw pathfinding edges
    vec2<int> pf_edge_adj = {GRIDSIZE/2, GRIDSIZE/2};
    for (size_t rid = 0; rid < pf_data.edges.size(); ++rid) {
        for (size_t i = 0; i < pf_data.edges[rid].size(); ++i) {
            Line my_line = {GRIDSIZE*pf_data.edges[rid][i].start + pf_edge_adj, GRIDSIZE*pf_data.edges[rid][i].end + pf_edge_adj};
            my_line.start -= offset;
            my_line.end -= offset;
            draw_line(my_line, PATH_EDGE_COL);
        }
    }

    // draw pathfinding nodes
    vec2<int> pf_node_adj = {GRIDSIZE/2 - PF_NODE_RADIUS/2, GRIDSIZE/2 - PF_NODE_RADIUS/2};
    for (size_t rid = 0; rid < pf_data.nodes.size(); ++rid) {
        for (size_t i = 0; i < pf_data.nodes[rid].size(); ++i) {
            Rect my_rect = {GRIDSIZE*pf_data.nodes[rid][i] + pf_node_adj, {PF_NODE_RADIUS, PF_NODE_RADIUS}};
            my_rect.position -= offset;
            draw_rect(my_rect, PATH_NODE_COL, true);
        }
    }
}
