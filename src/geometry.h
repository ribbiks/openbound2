#pragma once
#include "Array2D.h"
#include "misc_gfx.h"
#include "Vec2.h"

bool line_of_sight(float x1, float y1, float x2, float y2, const Array2D<bool>& wall_dat);
int cross(const vec2<int>& a, const vec2<int>& b);
bool points_are_collinear(const vec2<int>& a, const vec2<int>& b, const vec2<int>& c);
bool point_is_on_line_segment(const vec2<int>& p, const Line& line);
bool line_contains_line(const Line& line1, const Line& line2);

#define SIGN(x) (x > 0 ? 1 : (x < 0 ? -1 : 0))
#define FRAC0(x) (x - floorf(x))
#define FRAC1(x) (1 - x + floorf(x))
