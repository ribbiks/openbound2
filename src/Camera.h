#pragma once
#include <array>
#include <cmath>
#include <algorithm>

#include "Vec2.h"

static const float CAMERA_NUDGE_SCALAR = 800.0f;
static const float CAMERA_MAX_SPEED = 64.0f;
static const float CAMERA_MOVE_SCALAR = 0.30f;
static const float CAMERA_MOVE_AFFINE = 5.00f;
static const float CAMERA_MIN_MOVE_AMOUNT = 1.00f;

class Camera {
private:
    vec2<float> pos;
    vec2<float> target;
    std::array<float,4> bounds;
    bool locked_x;
    bool locked_y;

public:
    Camera();
    Camera(const vec2<float>& pos, const vec2<float>& target, const std::array<float,4>& bounds);
    void set_target(const vec2<float>& new_target);
    void reset(const vec2<float>& new_target);
    void nudge_target(bool move_up, bool move_down, bool move_left, bool move_right, float delta_time);
    void set_bounds(const vec2<int>& xrange, const vec2<int>& yrange);
    void update(float delta_time);
    vec2<int> get_pos();
    vec2<int> get_target();
};
