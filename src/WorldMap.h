#pragma once
#include <fstream>
#include <stdexcept>
#include <vector>

#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>

#include "Array2D.h"
#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"
#include "pathfinding.h"
#include "Vec2.h"

using json = nlohmann::json;

extern SDL_Renderer* renderer;

class WorldMap {
private:
    PathfindingData pf_data;
    Array2D<bool> wall_dat;
    vec2<int> player_start;
    std::string map_name;

public:
    WorldMap(const std::string& path)
    {
        // load map from json
        std::ifstream input_file(path);
        json loaded_data = json::parse(input_file);
        //
        map_name = loaded_data["map_name"];
        int map_width = loaded_data["map_width"];
        int map_height = loaded_data["map_height"];
        if (map_width < 1 or map_height < 1)
            throw std::invalid_argument("Map has invalid dimensions");
        //
        Array2D<int> tile_dat(map_width, map_height, loaded_data["tile_dat"].get<std::vector<int>>());
        wall_dat = Array2D<bool>(tile_dat.width(), tile_dat.height(), false);
        for (int i = 0; i < tile_dat.width(); ++i) {
            for (int j = 0; j < tile_dat.height(); ++j) {
                // eventually replace this with a dictionary of tile properties
                if (tile_dat[i][j] > 0)
                    wall_dat[i][j] = true;
            }
        }
        //
        std::vector<int> start_pos = loaded_data["start_pos"].get<std::vector<int>>();
        if (start_pos.size() != 2 || start_pos[0] < 0 || start_pos[1] < 0)
            throw std::invalid_argument("Map has invalid start_pos");
        player_start = {start_pos[0], start_pos[1]};
        //
        printf("map_name: %s (%ix%i)\n", map_name.c_str(), tile_dat.width(), tile_dat.height());
        printf("player_start: (%i,%i)\n", player_start.x, player_start.y);

        double start_time = SDL_GetTicks() / 1000.0;
        pf_data = get_pathfinding_data(wall_dat);
        double end_time = SDL_GetTicks() / 1000.0 - start_time;
        printf("map processed in %f seconds\n", end_time);
    }

    vec2<int> get_map_size() {
        return vec2<int>(GRIDSIZE*wall_dat.width(), GRIDSIZE*wall_dat.height());
    }

    vec2<int> get_start_pos() {
        return GRIDSIZE * player_start + vec2<int>({GRIDSIZE/2, GRIDSIZE/2});
    }

    void update() {
        //
    }

    void draw(const vec2<int>& offset) {
        // draw impassable tiles
        for (int i = 0; i < wall_dat.width(); ++i) {
            for (int j = 0; j < wall_dat.height(); ++j) {
                if (wall_dat[i][j]) {
                    Rect my_rect = {GRIDSIZE*vec2<int>(i, j), {GRIDSIZE, GRIDSIZE}};
                    my_rect.position -= offset;
                    draw_rect(my_rect, WALL_TILE_COL, true);
                }
            }
        }
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
};
