#pragma once
#include <array>
#include <string>
#include <queue>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

static const std::array<float,7> MOVE_CYCLE = {2.0f, 8.0f, 9.0f, 5.0f, 6.0f, 7.0f, 2.0f};
static const int TURN_SPEED = 40;

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
    std::queue<PlayerOrder> incoming_orders;

public:
    //// default constructor
    //Mauzling() :
    //    player_position(0.0f, 0.0f),
    //    player_angle(0.0f),
    //    player_radius(PLAYER_RADIUS),
    //    surface_debug(nullptr),
    //    surface_ellipse(nullptr),
    //    player_state(0),
    //    iscript_ind(0),
    //    player_is_selected(false),
    //    incoming_orders() {}

    Mauzling(const vec2<float>& pos, std::string image_filename) :
        player_position(pos),
        player_angle(angle_clamp(45.0f)),
        player_radius(PLAYER_RADIUS),
        player_state(0),
        iscript_ind(0),
        player_is_selected(true),
        incoming_orders() {

        // placeholder graphic
        surface_debug = IMG_Load(image_filename.c_str());
        if (surface_debug == nullptr) {
            SDL_Log("Unable to load image %s! SDL_image Error: %s\n", image_filename.c_str(), IMG_GetError());
            return;
        }
        SDL_SetColorKey(surface_debug, SDL_TRUE, SDL_MapRGB(surface_debug->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));

        // selection ellipse graphic
        int ellipse_width = 2 * player_radius + 5;
        int ellipse_height = 13;
        surface_ellipse = draw_ellipse(ellipse_width, ellipse_height, UNIT_ELLIPSE_COL);
    }

    ~Mauzling() {
        if (surface_debug)
            SDL_FreeSurface(surface_debug);
    }

    void get_turn_angles(const vec2<float>& start_position, const vec2<float>& goal_position, float start_angle) {
        vec2<float> dv = goal_position - start_position;
        float g_ang = angle_clamp(-std::atan2(dv.y, dv.x) * RADIAN_SCALAR);
        float d_ang = std::fmod(g_ang - start_angle + 180.0f, 360.0f) - 180.0f;
        bool is_clockwise = d_ang > 0;
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

    void update() {
        //
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
        incoming_orders.push({order_coordinates, MOVE_DELAY, move_is_queue});
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
            SDL_Rect rect = {static_cast<int>(player_position.x - player_radius - 1 - offset.x),
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
        }
    }
};
