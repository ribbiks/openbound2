#pragma once
#include <vector>

#include "Array2D.h"
#include "Vec2.h"

struct Line {
    vec2<int> start;
    vec2<int> end;
};

struct FLine {
    vec2<float> start;
    vec2<float> end;
};

struct Rect {
    vec2<int> position;  // top-left corner
    vec2<int> size;      // width and height
};

struct FRect {
    vec2<float> position;  // top-left corner
    vec2<float> size;      // width and height
};

const vec2<int> NULL_VEC = {-99999, -99999};

std::vector<vec2<int>> dda_grid_traversal(float x1, float y1, float x2, float y2);
bool points_are_visible_to_eachother(const vec2<float>& p1, const vec2<float>& p2, const Array2D<bool>& wall_dat);
int cross(const vec2<int>& a, const vec2<int>& b);
bool points_are_collinear(const vec2<int>& a, const vec2<int>& b, const vec2<int>& c);
bool point_is_on_line_segment(const vec2<int>& p, const Line& line);
bool point_in_box(const vec2<float>& point, const FRect& box);
bool line_contains_line(const Line& line1, const Line& line2);
int value_clamp(int v, int v_min, int v_max);
float value_clamp(float v, float v_min, float v_max);
float angle_clamp(float angle);

#define SIGN(x) (x > 0 ? 1 : (x < 0 ? -1 : 0))
#define FRAC0(x) (x - floorf(x))
#define FRAC1(x) (1 - x + floorf(x))
