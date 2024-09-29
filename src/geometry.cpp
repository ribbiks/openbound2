#include "geometry.h"

//
// uses DDA algorithm for grid traversal (adapated from a 3d voxel version)
//
bool line_of_sight(float x1, float y1, float x2, float y2, const Array2D<bool>& wall_dat) {
    float tMaxX, tMaxY, tDeltaX, tDeltaY;
    vec2<int> voxel; 

    int dx = SIGN(x2 - x1);
    if (dx != 0)
        tDeltaX = fmin(dx / (x2 - x1), 10000000.0f);
    else
        tDeltaX = 10000000.0f;
    if (dx > 0)
        tMaxX = tDeltaX * FRAC1(x1);
    else
        tMaxX = tDeltaX * FRAC0(x1);
    voxel.x = static_cast<int>(x1);

    int dy = SIGN(y2 - y1);
    if (dy != 0)
        tDeltaY = fmin(dy / (y2 - y1), 10000000.0f);
    else
        tDeltaY = 10000000.0f;
    if (dy > 0)
        tMaxY = tDeltaY * FRAC1(y1);
    else
        tMaxY = tDeltaY * FRAC0(y1);
    voxel.y = static_cast<int>(y1);

    while (true) {
        if (tMaxX < tMaxY) {
            voxel.x += dx;
            tMaxX += tDeltaX;
        }
        else {
            voxel.y += dy;
            tMaxY += tDeltaY;
        }
        if (tMaxX > 1 && tMaxY > 1)
            break;
        //
        if (wall_dat[voxel.x][voxel.y])
            return false;
    }
    return true;
}

int cross(const vec2<int>& a, const vec2<int>& b) {
    return a.x * b.y - a.y * b.x;
}

bool points_are_collinear(const vec2<int>& a, const vec2<int>& b, const vec2<int>& c) {
    return cross({b.x - a.x, b.y - a.y}, {c.x - a.x, c.y - a.y}) == 0;
}

bool point_is_on_line_segment(const vec2<int>& p, const Line& line) {
    if (!points_are_collinear(line.start, line.end, p))
        return false;
    return p.x >= std::min(line.start.x, line.end.x) &&
           p.x <= std::max(line.start.x, line.end.x) &&
           p.y >= std::min(line.start.y, line.end.y) &&
           p.y <= std::max(line.start.y, line.end.y);
}

bool point_in_box(const vec2<float>& point, const FRect& box) {
    float right = box.position.x + box.size.x;
    float bottom = box.position.y + box.size.y;
    return (point.x >= box.position.x && point.x <= right &&
            point.y >= box.position.y && point.y <= bottom);
}

// does line1 contain line2?
bool line_contains_line(const Line& line1, const Line& line2) {
    return points_are_collinear(line1.start, line1.end, line2.start) &&
           points_are_collinear(line1.start, line1.end, line2.end) &&
           point_is_on_line_segment(line2.start, line1) &&
           point_is_on_line_segment(line2.end, line1);
}

int value_clamp(int v, int v_min, int v_max) {
    return std::min(std::max(v, v_min), v_max);
}

float value_clamp(float v, float v_min, float v_max) {
    return std::min(std::max(v, v_min), v_max);
}

float angle_clamp(float angle) {
    return std::fmod(std::fmod(angle, 360.0f) + 360.0f, 360.0f);
}
