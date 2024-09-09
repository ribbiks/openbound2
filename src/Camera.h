#pragma once

#include <array>
#include <cmath>
#include <algorithm>

#include "globals.h"
#include "Vec2.h"

const float CAMERA_NUDGE_SCALAR = 800.0f;
const float CAMERA_MAX_SPEED = 64.0f;
const float CAMERA_MOVE_SCALAR = 0.30f;
const float CAMERA_MOVE_AFFINE = 5.00f;
const float CAMERA_MIN_MOVE_AMOUNT = 1.00f;

float value_clamp(float v, float v_min, float v_max) {
    return std::min(std::max(v, v_min), v_max);
}

class Camera {
public:
    // default constructor
    Camera() :
        pos(0.0f, 0.0f), 
        target(0.0f, 0.0f), 
        bounds{0.0f, static_cast<float>(RESOLUTION.x), 0.0f, static_cast<float>(RESOLUTION.y)},
        locked_x(false), 
        locked_y(false) {}

    Camera(const vec2<float>& pos, const vec2<float>& target, const std::array<float, 4>& bounds) :
        pos(pos),
        target(target),
        bounds(bounds),
        locked_x(false),
        locked_y(false) {}

    void set_target(const vec2<float>& new_target) {
        target = new_target;
    }

    void nudge_target(bool move_up, bool move_down, bool move_left, bool move_right, float delta_time) {
        if (move_up && !move_down)
            target -= {0.0f, CAMERA_NUDGE_SCALAR * delta_time};
        else if (move_down && !move_up)
            target += {0.0f, CAMERA_NUDGE_SCALAR * delta_time};
        if (move_left && !move_right)
            target -= {CAMERA_NUDGE_SCALAR * delta_time, 0.0f};
        else if (move_right && !move_left)
            target += {CAMERA_NUDGE_SCALAR * delta_time, 0.0f};
        target.x = value_clamp(target.x, bounds[0], bounds[1]);
        target.y = value_clamp(target.y, bounds[2], bounds[3]);
    }

    void set_bounds(vec2<int> xrange, vec2<int> yrange) {
        bounds[0] = static_cast<float>(xrange.x);
        bounds[1] = static_cast<float>(xrange.y) - static_cast<float>(RESOLUTION.x);
        bounds[2] = static_cast<float>(yrange.x);
        bounds[3] = static_cast<float>(yrange.y) - static_cast<float>(RESOLUTION.y);
        if (bounds[1] <= bounds[0])
            locked_x = true;
        if (bounds[3] <= bounds[2])
            locked_y = true;
    }

    void update(float delta_time) {
        if (pos != target) {
            vec2<float> d = target - pos;
            if (locked_x)
                d.x = 0.0f;
            if (locked_y)
                d.y = 0.0f;
            float dl = std::sqrt(d.x * d.x + d.y * d.y);
            float move_amt = dl * dl * delta_time * CAMERA_MOVE_SCALAR + delta_time * CAMERA_MOVE_AFFINE;
            move_amt = value_clamp(move_amt, 0.0f, CAMERA_MAX_SPEED);
            //
            if (dl > CAMERA_MIN_MOVE_AMOUNT) {
                d.x = (d.x / dl) * move_amt;
                d.y = (d.y / dl) * move_amt;
                pos += d;
                pos.x = value_clamp(pos.x, bounds[0], bounds[1]);
                pos.y = value_clamp(pos.y, bounds[2], bounds[3]);
            }
            else {
                pos = target;
            }
        }
    }

    vec2<int> get_pos() const {
        return vec2<int>(std::round(pos.x), std::round(pos.y));
    }

    vec2<int> get_target() const {
        return vec2<int>(std::round(target.x), std::round(target.y));
    }

private:
    vec2<float> pos;
    vec2<float> target;
    std::array<float, 4> bounds;
    bool locked_x;
    bool locked_y;
};
