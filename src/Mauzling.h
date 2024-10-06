#pragma once
#include <algorithm>
#include <array>
#include <string>
#include <queue>

#include <SDL.h>

#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"
#include "Vec2.h"
#include "WorldMap.h"

extern SDL_Renderer* renderer;

static const std::array<float,7> MOVE_CYCLE = {2.0f, 8.0f, 9.0f, 5.0f, 6.0f, 7.0f, 2.0f};
static const float TURN_SPEED = 40; // degrees per tick

//
// pathing / click delay stuff
//
static const int MOVE_DELAY             = 4;    // OpenBound "turnrate"
static const int QUEUE_DELAY            = 1;
static const size_t MAX_ORDERS_IN_QUEUE = 32;
static const int CLICK_DEADZONE         = 4;

static const vec2<float> DEADZONE_VEC2 = {4.0f, 4.0f};
static const vec2<float> CLICKSELECTION_VEC2 = {8.0f, 8.0f};

struct PlayerState {
    static const int IDLE    = 0;     // idle
    static const int DELAY   = 1;     // received orders, but we're waiting before we accept them (to emulate click delay)
    static const int TURNING = 2;     // turning
    static const int MOVING  = 3;     // moving
    static const int ARRIVED = 4;     // destination reached, next frame we will process our next order if we have one
    static const int DELAY_Q = 5;     // we have an order in queue, but are waiting to accept it (to emulate shift-click delay)
    static const int DEAD    = 6;     // out of lives, do not revive
};

struct PlayerOrder {
    vec2<int> coordinates;
    int current_delay;
    bool is_queue;
};

struct AcceptedOrder {
    vec2<int> goal_coordinates;
    int accept_delay;
    bool request_new_paths;
    vec2<int> clicked_coordinates;
};

static const vec2<int> NO_CLICKPOS = {-999, -999};

class Mauzling {
private:
    vec2<float> player_position;
    float player_angle;
    float player_radius;
    SDL_Surface* surface_debug;
    SDL_Surface* surface_ellipse;
    int player_state;
    int iscript_ind;
    bool player_is_selected;
    std::vector<PlayerOrder> incoming_orders;
    std::queue<AcceptedOrder> order_queue;
    std::queue<float> turns_we_need_to_do;

    int get_turn_delay(float d_angle) {
        d_angle = std::abs(d_angle);
        if (d_angle <= TURN_SPEED) {
            return (player_state == PlayerState::MOVING || player_state == PlayerState::TURNING) ? 0 : 1;
        }
        return static_cast<int>(d_angle / TURN_SPEED);
    }

    std::queue<float> get_turn_angles(const vec2<float>& start_position, const vec2<float>& goal_position, float start_angle, const vec2<int> clickpos = NO_CLICKPOS) {
        vec2<float> dv = goal_position - start_position;
        float g_ang = angle_clamp(-std::atan2(dv.y, dv.x) * RADIAN_SCALAR);
        float d_ang = angle_clamp(g_ang - start_angle);
        if (d_ang > 180.0f)
            d_ang -= 360.0f;
        bool is_clockwise = d_ang < 0;

        // if player is trying to do a 180 turn against a wall, turn towards the wall (it looks bad otherwise)
        if (d_ang > 180.f - EPSILON) {
            vec2<float> d_vec2 = clickpos - start_position;
            float g_ang2 = -std::atan2(d_vec2.y, d_vec2.x) * RADIAN_SCALAR;
            float d_ang2 = g_ang2 - start_angle;
            d_ang2 = std::fmod(d_ang2 + 540.0f, 360.0f) - 180.0f;
            is_clockwise = d_ang2 < 0;
        }

        int num_turn_frames = get_turn_delay(std::abs(d_ang));
        std::queue<float> upcoming_angs;
        if (num_turn_frames > 1) {
            float turn_increment = is_clockwise ? -TURN_SPEED : TURN_SPEED;
            float current_angle = start_angle;
            for (int i = 1; i < num_turn_frames; ++i) {
                current_angle = angle_clamp(current_angle + turn_increment);
                upcoming_angs.push(current_angle);
            }
        }
        else if (num_turn_frames == 1) {
            if (std::abs(d_ang) <= TURN_SPEED)
                upcoming_angs.push(g_ang);
            else {
                float turn_increment = is_clockwise ? -TURN_SPEED : TURN_SPEED;
                upcoming_angs.push(angle_clamp(start_angle + turn_increment));
            }
        }
        upcoming_angs.push(g_ang);
        return upcoming_angs;
    }

    void reset_iscript() {
        iscript_ind = 0;
    }

    void increment_iscript() {
        iscript_ind = (iscript_ind + 1) % MOVE_CYCLE.size();
    }

    float get_current_speed() {
        return MOVE_CYCLE[iscript_ind];
    }

public:
    Mauzling(const vec2<float>& pos, std::string image_filename) :
        player_position(pos),
        player_angle(angle_clamp(0.0f)),
        player_radius(PLAYER_RADIUS),
        player_state(0),
        iscript_ind(0),
        player_is_selected(false),
        incoming_orders(),
        order_queue(),
        turns_we_need_to_do() {

        // placeholder graphic
        surface_debug = load_image(image_filename);

        // selection ellipse graphic
        int ellipse_width = 2 * player_radius + 5;
        int ellipse_height = 13;
        surface_ellipse = draw_ellipse(ellipse_width, ellipse_height, UNIT_ELLIPSE_COL);
    }

    ~Mauzling() {
        if (surface_debug)
            SDL_FreeSurface(surface_debug);
        if (surface_ellipse)
            SDL_FreeSurface(surface_ellipse);
    }

    void update_position(const vec2<float>& pos) {
        player_position = pos;
    }

    void update_angle(const float angle) {
        player_angle = angle_clamp(angle);
    }

    void check_selection_click(const vec2<int>& clickpos) {
        if (player_state != PlayerState::DEAD) {
            FRect playerbox;
            playerbox.position = player_position - CLICKSELECTION_VEC2;
            playerbox.size = 2 * CLICKSELECTION_VEC2;
            if (point_in_box(clickpos, playerbox))
                player_is_selected = true;
            else
                player_is_selected = false;
        }
    }

    void check_selection_box(const vec2<int>& point1, const vec2<int>& point2) {
        if (player_state != PlayerState::DEAD) {
            vec2<float> p1 = {static_cast<float>(std::min(point1.x, point2.x)), static_cast<float>(std::min(point1.y, point2.y))};
            vec2<float> p2 = {static_cast<float>(std::max(point1.x, point2.x)), static_cast<float>(std::max(point1.y, point2.y))};
            FRect selection_box;
            selection_box.position = p1;
            selection_box.size = p2 - p1;
            if (point_in_box(player_position, selection_box))
                player_is_selected = true;
            else
                player_is_selected = false;
        }
    }

    void tick(WorldMap* world_map) {
        if (player_state == PlayerState::IDLE || player_state == PlayerState::DELAY_Q)
            reset_iscript();
        if (player_state == PlayerState::ARRIVED)
            player_state = PlayerState::IDLE;
        //
        // nudge our position if we're on a conveyor
        //
        player_position = world_map->get_scrolled_pos(player_position);
        //
        // decrement delay on incoming orders. if any are ready add them to queue
        //
        for (size_t i = 0; i < incoming_orders.size(); ++i) {
            incoming_orders[i].current_delay -= 1;
            if (incoming_orders[i].current_delay <= 0) {
                AcceptedOrder accepted_order = {incoming_orders[i].coordinates, 0, true, NO_CLICKPOS};
                // for shift-clicks delay will be QUEUE_DELAY, for subpaths of a larger path it will be 0
                if (incoming_orders[i].is_queue && !order_queue.empty())
                    accepted_order.accept_delay = QUEUE_DELAY;
                else
                    order_queue = std::queue<AcceptedOrder>();
                order_queue.push(accepted_order);
            }
        }
        incoming_orders.erase(std::remove_if(incoming_orders.begin(), incoming_orders.end(), [](PlayerOrder n) { return n.current_delay <= 0; }), incoming_orders.end());
        //
        // act out our current order if we have one
        //
        if (!order_queue.empty()) {
            order_queue.front().accept_delay -= 1;
            if (order_queue.front().accept_delay > -1)
                player_state = PlayerState::DELAY_Q;
            else {
                // order accepted, pathfind subpaths if this is a new command
                bool pathfind_success = true;
                if (order_queue.front().request_new_paths) {
                    pathfind_success = false;
                    vec2<int> clicked_pos = order_queue.front().goal_coordinates;
                    //
                    double start_time = SDL_GetTicks() / 1000.0;
                    std::vector<vec2<int>> waypoints = world_map->pathfind(player_position, order_queue.front().goal_coordinates);
                    double end_time = SDL_GetTicks() / 1000.0 - start_time;
                    printf("pathfinding completed in %f seconds\n", end_time);
                    //
                    if (!waypoints.empty()) {
                        pathfind_success = true;
                        vec2<float> dv = vec2<float>(waypoints[0]) - player_position;
                        // if we're already at the destination then we just need to turn
                        if (dv.length() < 0.01f) {
                            turns_we_need_to_do = get_turn_angles(player_position, order_queue.front().goal_coordinates, player_angle);
                            order_queue.front() = {player_position, -2, false, NO_CLICKPOS}; // why was this -2 and not -1 ?
                        }
                        // otherwise assign all the subpaths as new move orders
                        else {
                            order_queue.pop();
                            // prepend waypoints to order_queue
                            std::queue<AcceptedOrder> new_queue;
                            for (const auto& waypoint : waypoints)
                                new_queue.push({waypoint, 0, false, clicked_pos});
                            std::queue<AcceptedOrder> temp_queue = order_queue;
                            while (!temp_queue.empty()) {
                                new_queue.push(temp_queue.front());
                                temp_queue.pop();
                            }
                            order_queue = new_queue;
                            order_queue.front().accept_delay = -1; // so we process the first subpath immediately
                        }
                    }
                }
                // abandon this order if no path was returned
                if (!pathfind_success) {
                    order_queue.pop();
                    player_state = PlayerState::ARRIVED;
                }
                else {
                    // lets compute necessary turns before we can begin moving
                    if (order_queue.front().accept_delay == -1)
                        turns_we_need_to_do = get_turn_angles(player_position, order_queue.front().goal_coordinates, player_angle, order_queue.front().clicked_coordinates);
                    // do the actual turning
                    if (!turns_we_need_to_do.empty()) {
                        player_state = PlayerState::TURNING;
                        player_angle = angle_clamp(turns_we_need_to_do.front());
                        turns_we_need_to_do.pop();
                        if (turns_we_need_to_do.empty())
                            player_state = PlayerState::MOVING;
                        else
                            increment_iscript();
                    }
                    // move if we're now ready
                    if (player_state == PlayerState::MOVING) {
                        vec2<float> fgoal = vec2<float>(order_queue.front().goal_coordinates);
                        vec2<float> dv = fgoal - player_position;
                        float dv_length = dv.length();
                        float move_amount = get_current_speed();
                        if (dv_length <= move_amount) {
                            player_position = fgoal;
                            order_queue.pop();
                            player_state = PlayerState::ARRIVED;
                        }
                        else
                            player_position += (move_amount / dv_length) * dv;
                        increment_iscript();
                    }
                }
            }
        }
    }

    // returns true if a cursor flash should be drawn
    bool issue_new_order(const vec2<int>& order_coordinates, bool shift_pressed) {
        // reject order if player is not selected
        if (!player_is_selected)
            return false;
        // reject order if clicked inside player
        vec2<float> forder_coords = order_coordinates;
        FRect deadzone;
        deadzone.position = player_position - DEADZONE_VEC2;
        deadzone.size = 2 * DEADZONE_VEC2;
        if (point_in_box(forder_coords, deadzone))
            return false;
        // new order or queue order?
        bool move_is_queue = false;
        if (shift_pressed) {
            if (incoming_orders.size() >= MAX_ORDERS_IN_QUEUE)
                return true;
            move_is_queue = true;
        }
        // don't accept redundant orders
        if (!incoming_orders.empty() && order_coordinates == incoming_orders.back().coordinates)
            return true;
        // don't accept new orders that are close to being redundant
        if (!incoming_orders.empty() && !move_is_queue) {
            int dx = std::abs(incoming_orders.back().coordinates.x - order_coordinates.x);
            int dy = std::abs(incoming_orders.back().coordinates.y - order_coordinates.y);
            if (dx <= CLICK_DEADZONE && dy <= CLICK_DEADZONE)
                return true;
        }
        // append order to queue
        incoming_orders.push_back({order_coordinates, MOVE_DELAY, move_is_queue});
        if (player_state == PlayerState::IDLE)
            player_state = PlayerState::DELAY;
        return true;
    }

    void draw(const vec2<int>& offset) {
        if (player_is_selected && surface_ellipse) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface_ellipse);
            if (texture == nullptr) {
                SDL_Log("Unable to create texture! SDL Error: %s\n", SDL_GetError());
                return;
            }
            // fiddle with these numbers until it's aligned properly
            SDL_Rect rect = {static_cast<int>(player_position.x - player_radius - 2 - offset.x),
                             static_cast<int>(player_position.y + 2 - offset.y),
                             surface_ellipse->w,
                             surface_ellipse->h};
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        if (surface_debug) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface_debug);
            if (texture == nullptr) {
                SDL_Log("Unable to create texture! SDL Error: %s\n", SDL_GetError());
                return;
            }
            SDL_Rect rect = {static_cast<int>(player_position.x - player_radius - offset.x),
                             static_cast<int>(player_position.y - player_radius - offset.y),
                             static_cast<int>(player_radius * 2),
                             static_cast<int>(player_radius * 2)};
            SDL_RenderCopyEx(renderer, texture, nullptr, &rect, -player_angle, nullptr, SDL_FLIP_NONE);
            SDL_DestroyTexture(texture);
            //
            draw_rect(rect, UNIT_HITBOX);
        }
    }
};
