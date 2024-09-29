#pragma once
#include <unordered_map>
#include <vector>

#include "Array2D.h"
#include "geometry.h"
#include "Vec2.h"

struct PathfindingData {
    Array2D<int> tile_2_region_id;
    int num_regions;
    std::vector<std::vector<vec2<int>>> nodes;
    std::vector<std::vector<Line>> edges;
    std::vector<std::unordered_map<int, std::vector<int>>> graphs;
};

struct BlockedDirections {
    static const int NW = 1;
    static const int NE = 2;
    static const int SW = 4;
    static const int SE = 8;
};

bool line_of_sight_unit(const vec2<float>& v1, const vec2<float>& v2, const Array2D<bool>& wall_dat);
bool edge_has_good_incoming_angles(const vec2<int>& v1, const vec2<int>& v2, int corner1, int corner2);
bool edge_never_turns_towards_wall(const vec2<int>& v1, const vec2<int>& v2, int corner1, int corner2);
PathfindingData get_pathfinding_data(const Array2D<bool>& wall_dat);
std::vector<vec2<int>> get_pathfinding_waypoints(const vec2<int>& start_pos, const vec2<int>& end_pos, const PathfindingData& pf_data, const Array2D<bool>& wall_dat);
