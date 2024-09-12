#pragma once
#include <algorithm>
#include <vector>
#include <queue>

#include "Array2D.h"
#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"
#include "Vec2.h"

struct PathfindingData {
    Array2D<int> tile_2_region_id;
    int num_regions;
    std::vector<std::vector<vec2<int>>> nodes;
    std::vector<std::vector<Line>> edges;
};

enum {
    BLOCKED_NW = 1,
    BLOCKED_NE = 2,
    BLOCKED_SW = 4,
    BLOCKED_SE = 8
};

bool line_of_sight_unit(const vec2<int>& v1, const vec2<int>& v2, const Array2D<bool>& wall_dat);
bool edge_has_good_incoming_angles(const vec2<int>& v1, const vec2<int>& v2, int corner1, int corner2);
bool edge_never_turns_towards_wall(const vec2<int>& v1, const vec2<int>& v2, int corner1, int corner2);
PathfindingData get_pathfinding_data(const Array2D<bool>& wall_dat);
