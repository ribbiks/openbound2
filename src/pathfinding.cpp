#include "pathfinding.h"

#include <algorithm>
#include <queue>

bool line_of_sight_unit(const vec2<float>& v1, const vec2<float>& v2, const Array2D<bool>& wall_dat) {
    for (const auto& adj : ADJ_LOS_UNIT) {
        if (!points_are_visible_to_eachother({v1.x + adj.x, v1.y + adj.y}, {v2.x + adj.x, v2.y + adj.y}, wall_dat))
            return false;
    }
    return true;
}

bool valid_player_position(const vec2<int>& position, const Array2D<bool>& wall_dat) {
    for (const auto& adj : ADJ_VALIDPOS) {
        vec2<int> v_adj = position + adj;
        vec2<int> v_map = {v_adj.x / GRIDSIZE, v_adj.y / GRIDSIZE};
        if (wall_dat[v_map.x][v_map.y])
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
    Array2D<bool> visited(width, height, false);
    int num_regions = 0;

    for (int x = 1; x < width - 1; ++x) {
        for (int y = 1; y < height - 1; ++y) {
            if (!visited[x][y] && !wall_dat[x][y]) {
                visited[x][y] = true;
                tile_2_region_id[x][y] = num_regions;
                
                std::queue<vec2<int>> queue;
                queue.push({x,y});
                while (!queue.empty()) {
                    vec2<int> current = queue.front();
                    queue.pop();
                    for (const auto& dir : MOVE_DIR) {
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
    std::vector<std::unordered_map<int, std::vector<GraphNode>>> graphs;
    for (int rid = 0; rid < num_regions; ++rid) {
        std::vector<Line> candidate_edges, filtered_edges;
        std::vector<vec2<int>> node_ij;
        std::vector<float> node_dists;
        std::unordered_map<int, std::vector<GraphNode>> graph;
        int num_nodes = nodes[rid].size();
        int filtcount1 = 0;
        int filtcount2 = 0;
        int filtcount3 = 0;
        int filtcount4 = 0;
        for (int i = 0; i < num_nodes; ++i) {
            for (int j = i+1; j < num_nodes; ++j) {
                vec2<int> v1 = nodes[rid][i];
                vec2<int> v2 = nodes[rid][j];
                int corner1 = blocked_corners[rid][i];
                int corner2 = blocked_corners[rid][j];
                //
                if (edge_has_good_incoming_angles(v1, v2, corner1, corner2)) {
                    if (edge_never_turns_towards_wall(v1, v2, corner1, corner2)) {
                        vec2<float> v1f = {static_cast<float>(v1.x) + 0.5f, static_cast<float>(v1.y) + 0.5f};
                        vec2<float> v2f = {static_cast<float>(v2.x) + 0.5f, static_cast<float>(v2.y) + 0.5f};
                        if (line_of_sight_unit(v1f, v2f, wall_dat)) {
                            candidate_edges.push_back({v1, v2});
                            node_ij.push_back({i, j});
                            node_dists.push_back((v2f - v1f).length());
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
            if (!edge_contains_another_edge) {
                filtered_edges.push_back(candidate_edges[i]);
                graph[node_ij[i].x].push_back({node_ij[i].y, node_dists[i]});
                graph[node_ij[i].y].push_back({node_ij[i].x, node_dists[i]});
            }
            else
                filtcount4 += 1;
        }
        edges.push_back(filtered_edges);
        graphs.push_back(graph);
        //
        printf("region: %i (%zu edges, %i + %i + %i + %i filtered)\n", rid, filtered_edges.size(), filtcount1, filtcount2, filtcount3, filtcount4);
    }

    return {tile_2_region_id, num_regions, nodes, edges, graphs};
}

//
// big complicated function that does the actual pathfinding logic
//

std::vector<vec2<int>> get_pathfinding_waypoints(const vec2<int>& start_pos,
                                                 const vec2<int>& end_pos,
                                                 const PathfindingData& pf_data,
                                                 const Array2D<bool>& wall_dat) {
    std::vector<vec2<int>> waypoints;

    // check if we clicked in our current region
    vec2<int> map_coords_start = {start_pos.x / GRIDSIZE, start_pos.y / GRIDSIZE}; // (ux,uy)
    vec2<int> map_coords_end = {end_pos.x / GRIDSIZE, end_pos.y / GRIDSIZE};       // (cx,cy)
    int start_region = pf_data.tile_2_region_id[map_coords_start.x][map_coords_start.y];
    int end_region = pf_data.tile_2_region_id[map_coords_end.x][map_coords_end.y];
    //
    bool found_nearest_inbound_tile = false;
    if (start_region != end_region) {
        // draw a line from end_pos to click_pos, looking for a valid destination tile along the way
        //  - will modify map_coords_end if it finds a valid destination cell
        float x1 = static_cast<float>(map_coords_end.x) + 0.5f;
        float y1 = static_cast<float>(map_coords_end.y) + 0.5f;
        float x2 = static_cast<float>(start_pos.x) / F_GRIDSIZE;
        float y2 = static_cast<float>(start_pos.y) / F_GRIDSIZE;
        vec2<int> found_tile = NULL_VEC;
        std::vector<vec2<int>> voxels = dda_grid_traversal(x1, y1, x2, y2);
        for (size_t i = 0; i < voxels.size(); ++i) {
            if (pf_data.tile_2_region_id[voxels[i].x][voxels[i].y] == start_region) {
                found_tile = voxels[i];
                break;
            }
        }
        if (found_tile != NULL_VEC) {
            map_coords_end = found_tile;
            found_nearest_inbound_tile = true;
            printf("clicked out of bounds, using nearest tile [dda]: (%i,%i)\n", found_tile.x, found_tile.y);
        }
        else {
            // if line drawing failed, lets do a bfs to find the nearest valid tile
            int width = wall_dat.width();
            int height = wall_dat.height();
            Array2D<bool> visited(width, height, false); // this is inefficient
            std::queue<vec2<int>> queue;
            queue.push(map_coords_end);
            while (!queue.empty()) {
                vec2<int> current = queue.front();
                queue.pop();
                if (pf_data.tile_2_region_id[current.x][current.y] == start_region) {
                    found_tile = current;
                    break;
                }
                for (const auto& dir : MOVE_DIR) {
                    vec2<int> next = current + dir;
                    if (next.x >= 0 && next.x < width && next.y >= 0 && next.y < height &&
                        !visited[next.x][next.y] &&
                        (pf_data.tile_2_region_id[next.x][next.y] == -1 || pf_data.tile_2_region_id[next.x][next.y] == start_region)) {
                        visited[next.x][next.y] = true;
                        queue.push(next);
                    }
                }
            }
            if (found_tile != NULL_VEC) {
                map_coords_end = found_tile;
                found_nearest_inbound_tile = true;
                printf("clicked out of bounds, using nearest tile [bfs]: (%i,%i)\n", found_tile.x, found_tile.y);
            }
            // both strategies failed so we're just not going to move, sorry!
            else
                return waypoints;
        }
    }

    //
    // nudge end coordinates to a valid position when clicking near a wall
    //
    vec2<int> nudged_end_pos = end_pos;
    if (found_nearest_inbound_tile || !valid_player_position(end_pos, wall_dat)) {
        // starts with quantized pos --> nudges to desired pos
        vec2<int> nudged_pos = map_coords_end * GRIDSIZE + vec2<int>(GRIDSIZE/2, GRIDSIZE/2);
        //
        printf("nudging destination: (%i,%i) --> (%i,%i)", end_pos.x, end_pos.y, nudged_pos.x, nudged_pos.y);
        if (nudged_pos.x > end_pos.x) {
            while (nudged_pos.x > end_pos.x && valid_player_position({nudged_pos.x - 1, nudged_pos.y}, wall_dat))
                nudged_pos.x--;
        }
        else if (nudged_pos.x < end_pos.x) {
            while (nudged_pos.x < end_pos.x && valid_player_position({nudged_pos.x + 1, nudged_pos.y}, wall_dat))
                nudged_pos.x++;
        }
        if (nudged_pos.y > end_pos.y) {
            while (nudged_pos.y > end_pos.y && valid_player_position({nudged_pos.x, nudged_pos.y - 1}, wall_dat))
                nudged_pos.y--;
        }
        else if (nudged_pos.y < end_pos.y) {
            while (nudged_pos.y < end_pos.y && valid_player_position({nudged_pos.x, nudged_pos.y + 1}, wall_dat))
                nudged_pos.y++;
        }
        nudged_end_pos = nudged_pos;
        printf(" --> (%i,%i)\n", nudged_end_pos.x, nudged_end_pos.y);
    }

    //
    // check for a straight line between start and end
    //
    vec2<float> fcoords_start = {static_cast<float>(start_pos.x) / F_GRIDSIZE,
                                 static_cast<float>(start_pos.y) / F_GRIDSIZE};
    vec2<float> fcoords_end = {static_cast<float>(nudged_end_pos.x) / F_GRIDSIZE,
                               static_cast<float>(nudged_end_pos.y) / F_GRIDSIZE};
    if (line_of_sight_unit(fcoords_start, fcoords_end, wall_dat)) {
        waypoints.push_back(nudged_end_pos);
        return waypoints;
    }
    printf("OH NO...\n");

    //
    // pathfinding
    //
    auto graph = pf_data.graphs[start_region]; // we need a copy because we're going to modify it
    int num_nodes = pf_data.nodes[start_region].size();
    int starting_node = num_nodes;
    int ending_node = num_nodes+1;

    // insert start and end positions into graph
    // - also create astar heuristic since we're computing distances to the end node anyway
    graph[starting_node] = std::vector<GraphNode>();
    graph[ending_node] = std::vector<GraphNode>();
    std::vector<float> heuristic;
    for (int i = 0; i < num_nodes; ++i) {
        vec2<float> fcoords_node = {static_cast<float>(pf_data.nodes[start_region][i].x) + 0.5f,
                                    static_cast<float>(pf_data.nodes[start_region][i].y) + 0.5f};
        if (line_of_sight_unit(fcoords_start, fcoords_node, wall_dat)) {
            float dist_to_start_node = (fcoords_node - fcoords_start).length();
            graph[starting_node].push_back({i, dist_to_start_node});
            graph[i].push_back({starting_node, dist_to_start_node});
        }
        float dist_to_end_node = (fcoords_node - fcoords_end).length();
        heuristic.push_back(dist_to_end_node);
        if (line_of_sight_unit(fcoords_end, fcoords_node, wall_dat)) {
            graph[ending_node].push_back({i, dist_to_end_node});
            graph[i].push_back({ending_node, dist_to_end_node});
        }
    }
    heuristic.push_back((fcoords_start - fcoords_end).length()); // starting_node
    heuristic.push_back(0.0f);                                   // ending_node

    //for (size_t i = 0; i < pf_data.nodes[start_region].size(); ++i)
    //    printf("NODE %zu: (%i,%i)\n", i, pf_data.nodes[start_region][i].x, pf_data.nodes[start_region][i].y);
    
    //
    // astar
    //
    std::priority_queue<std::pair<int, float>, std::vector<std::pair<int, float>>, CompareNode> openSet;
    std::unordered_map<int, int> cameFrom;
    std::unordered_map<int, float> gScore;
    std::unordered_map<int, float> fScore;

    openSet.push({starting_node, 0});
    gScore[starting_node] = 0;
    fScore[starting_node] = heuristic[starting_node];

    std::vector<int> path;
    while (!openSet.empty()) {
        int current = openSet.top().first;
        //printf("INSIDE ASTAR: %i %i \n", current, ending_node);
        openSet.pop();

        if (current == ending_node) {
            while (current != starting_node) {
                path.push_back(current);
                current = cameFrom[current];
            }
            path.push_back(starting_node);
            std::reverse(path.begin(), path.end());
            break;
        }

        if (graph.find(current) == graph.end())
            continue;

        for (const auto& neighbor : graph.at(current)) {
            float tentative_gScore = gScore[current] + neighbor.dist;
            //printf("HELLO? %i g=%f\n", neighbor.node, tentative_gScore);
            if (gScore.find(neighbor.node) == gScore.end() || tentative_gScore < gScore[neighbor.node]) {
                cameFrom[neighbor.node] = current;
                gScore[neighbor.node] = tentative_gScore;
                fScore[neighbor.node] = gScore[neighbor.node] + heuristic[neighbor.node];
                //printf("HELLO! %i g=%f h=%f f=%f\n", neighbor.node, gScore[neighbor.node], heuristic[neighbor.node], fScore[neighbor.node]);
                openSet.push({neighbor.node, fScore[neighbor.node]});
            }
        }
    }

    for(size_t i = 0; i < path.size(); ++i){
        if (path[i] == ending_node)
            waypoints.push_back(nudged_end_pos);
        else if (path[i] != starting_node) {
            waypoints.push_back(pf_data.nodes[start_region][path[i]] * GRIDSIZE + vec2<int>(GRIDSIZE / 2, GRIDSIZE / 2));
        }
    }

    return waypoints;
}
