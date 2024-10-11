#pragma once
#include <string>
#include <vector>

#include "geometry.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;

struct ExplosionCount {
    std::vector<int> locs;
    std::vector<std::string> units;
    int delay;
};

class Obstacle {
private:
    int ob_num;
    Rect ob_startbox;
    Rect ob_endbox;
    vec2<int> ob_revive;

public:
    Obstacle(int num, Rect& startbox, Rect& endbox, vec2<int>& revive) :
        ob_num(num),
        ob_startbox(startbox),
        ob_endbox(endbox),
        ob_revive(revive) {
        //
    }
    ~Obstacle();
    void check_for_ob_start(const vec2<int>& player_pos);
    void check_for_ob_end(const vec2<int>& player_pos);
    void add_location();
    void add_event_explode_locs();
    void add_event_playsound();
    void add_event_teleport();
    void tick();
    void draw(const vec2<int>& offset);
};
