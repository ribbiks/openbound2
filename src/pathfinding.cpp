#include "pathfinding.h"

bool line_of_sight_unit(const vec2<int>& v1, const vec2<int>& v2, const Array2D<bool>& wall_dat) {
    const vec2<float> adj_list[] = {{PLAYER_RADIUS_EPSILON, PLAYER_RADIUS_EPSILON},
                                    {1.0f - PLAYER_RADIUS_EPSILON, PLAYER_RADIUS_EPSILON},
                                    {PLAYER_RADIUS_EPSILON, 1.0f - PLAYER_RADIUS_EPSILON},
                                    {1.0f - PLAYER_RADIUS_EPSILON, 1.0f - PLAYER_RADIUS_EPSILON}};
    for (const auto& adj : adj_list) {
        float x1 = static_cast<float>(v1.x) + adj.x;
        float y1 = static_cast<float>(v1.y) + adj.y;
        float x2 = static_cast<float>(v2.x) + adj.x;
        float y2 = static_cast<float>(v2.y) + adj.y;
        if (!line_of_sight(x1, y1, x2, y2, wall_dat))
            return false;
    }
    return true;
}

bool edge_has_good_incoming_angles(const vec2<int>& v1, const vec2<int>& v2, int corner1, int corner2) {
    if (v1.x == v2.x || v1.y == v2.y)
        return true;
    vec2<int> d = v2 - v1;
    bool edge1_clear = true;
    bool edge2_clear = true;
    // coming into v1 from v2
    if ((d.x < 0 && d.y < 0 && corner1 & BlockedDirections::SE) ||
        (d.x < 0 && d.y > 0 && corner1 & BlockedDirections::NE) ||
        (d.x > 0 && d.y > 0 && corner1 & BlockedDirections::NW) ||
        (d.x > 0 && d.y < 0 && corner1 & BlockedDirections::SW))
        edge1_clear = false;
    // coming into v2 from v1
    if ((d.x > 0 && d.y > 0 && corner2 & BlockedDirections::SE) ||
        (d.x > 0 && d.y < 0 && corner2 & BlockedDirections::NE) ||
        (d.x < 0 && d.y < 0 && corner2 & BlockedDirections::NW) ||
        (d.x < 0 && d.y > 0 && corner2 & BlockedDirections::SW))
        edge2_clear = false;
    return edge1_clear | edge2_clear;
}

bool edge_never_turns_towards_wall(const vec2<int>& v1, const vec2<int>& v2, int corner1, int corner2) {
    vec2<int> d = v2 - v1;
    vec2<int> corners = {corner1, corner2};
    if (std::abs(d.x) > std::abs(d.y)) {
        if (v1.x > v2.x)
            corners = {corner2, corner1};
        if ((corners.x == BlockedDirections::NW && corners.y == BlockedDirections::NE) ||
            (corners.x == BlockedDirections::SW && corners.y == BlockedDirections::SE))
            return false;
    }
    else {
        if (v1.y > v2.y)
            corners = {corner2, corner1};
        if ((corners.x == BlockedDirections::NW && corners.y == BlockedDirections::SW) ||
            (corners.x == BlockedDirections::NE && corners.y == BlockedDirections::SE))
            return false;
    }
    return true;
}

PathfindingData get_pathfinding_data(const Array2D<bool>& wall_dat) {

    //
    // PARSE MAP, GET DISCONNECTED REGIONS
    //

    int width = wall_dat.width();
    int height = wall_dat.height();
    
    Array2D<int> tile_2_region_id(width, height, -1);
    Array2D<bool> visited(width, height, 0);  // 0 for false, 1 for true
    int num_regions = 0;
    const vec2<int> directions[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (int x = 1; x < width - 1; ++x) {
        for (int y = 1; y < height - 1; ++y) {
            if (!visited[x][y] && !wall_dat[x][y]) {
                visited[x][y] = true;
                tile_2_region_id[x][y] = num_regions;
                
                std::queue<vec2<int>> queue;
                queue.push(vec2<int>(x, y));

                while (!queue.empty()) {
                    vec2<int> current = queue.front();
                    queue.pop();
                    for (const auto& dir : directions) {
                        vec2<int> next = current + dir;
                        if (next.x >= 0 && next.x < width && next.y >= 0 && next.y < height && !visited[next.x][next.y] && !wall_dat[next.x][next.y]) {
                            visited[next.x][next.y] = true;
                            tile_2_region_id[next.x][next.y] = num_regions;
                            queue.push(next);
                        }
                    }
                }
                ++num_regions;
            }
        }
    }
    printf("num_regions: %i\n", num_regions);

    //
    // GET CANDIDATE PATHING NODES
    //
    
    // nodes[region][i] = (x,y)
    std::vector<std::vector<vec2<int>>> nodes(num_regions);
    // blocked_corners[region][i] = val
    // - val & 1 --> NW is blocked
    // - val & 2 --> NE is blocked
    // - val & 4 --> SW is blocked --- note that these last two are swapped compared to openbound
    // - val & 8 --> SE is blocked ---
    std::vector<std::vector<int>> blocked_corners(num_regions);
    
    for (int x = 1; x < width - 1; ++x) {
        for (int y = 1; y < height - 1; ++y) {
            if (!wall_dat[x][y]) {
                bool a = !wall_dat[x-1][y-1];
                bool b = !wall_dat[x  ][y-1];
                bool c = !wall_dat[x+1][y-1];
                bool d = !wall_dat[x-1][y  ];
                bool e = !wall_dat[x+1][y  ];
                bool f = !wall_dat[x-1][y+1];
                bool g = !wall_dat[x  ][y+1];
                bool h = !wall_dat[x+1][y+1];

                int my_region_id = tile_2_region_id[x][y];
                if ((!a && b && d) || (!c && b && e) || (!f && d && g) || (!h && g && e)) {
                    nodes[my_region_id].push_back({x, y});
                    blocked_corners[my_region_id].push_back(0);
                    int vec_ind = blocked_corners[my_region_id].size() - 1;
                    if (!a) blocked_corners[my_region_id][vec_ind] += BlockedDirections::NW;
                    if (!c) blocked_corners[my_region_id][vec_ind] += BlockedDirections::NE;
                    if (!f) blocked_corners[my_region_id][vec_ind] += BlockedDirections::SW;
                    if (!h) blocked_corners[my_region_id][vec_ind] += BlockedDirections::SE;
                }
            }
        }
    }
    for (int rid = 0; rid < num_regions; ++rid) {
        printf("region: %i (%zu nodes)\n", rid, nodes[rid].size());
    }

    //
    // EDGE PRUNING 
    //

    std::vector<std::vector<Line>> edges;
    for (int rid = 0; rid < num_regions; ++rid) {
        std::vector<Line> candidate_edges, filtered_edges;
        int filtcount1 = 0;
        int filtcount2 = 0;
        int filtcount3 = 0;
        int filtcount4 = 0;
        for (size_t i = 0; i < nodes[rid].size(); ++i) {
            for (size_t j = i+1; j < nodes[rid].size(); ++j) {
                vec2<int> v1 = nodes[rid][i];
                vec2<int> v2 = nodes[rid][j];
                int corner1 = blocked_corners[rid][i];
                int corner2 = blocked_corners[rid][j];
                //
                if (edge_has_good_incoming_angles(v1, v2, corner1, corner2)) {
                    //
                    if (edge_never_turns_towards_wall(v1, v2, corner1, corner2)) {
                        //
                        if (line_of_sight_unit(v1, v2, wall_dat)) {
                            candidate_edges.push_back({v1, v2});
                        } else
                            filtcount3 += 1;
                    } else
                        filtcount2 += 1;
                } else
                    filtcount1 += 1;
            }
        }
        for (size_t i = 0; i < candidate_edges.size(); ++i) {
            bool edge_contains_another_edge = false;
            for (size_t j = 0; j < candidate_edges.size(); ++j) {
                if (i != j && line_contains_line(candidate_edges[i], candidate_edges[j])) {
                    edge_contains_another_edge = true;
                    break;
                }
            }
            if (!edge_contains_another_edge)
                filtered_edges.push_back(candidate_edges[i]);
            else
                filtcount4 += 1;
        }
        edges.push_back(filtered_edges);
        //
        printf("region: %i (%zu edges, %i + %i + %i + %i filtered)\n", rid, filtered_edges.size(), filtcount1, filtcount2, filtcount3, filtcount4);
    }

    return {tile_2_region_id, num_regions, nodes, edges};
}
