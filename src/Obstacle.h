#pragma once
#include <string>
#include <vector>

#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"
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
    bool action_moveplayer;
    int action_addlives;
    std::string action_changemusic;
    std::vector<Rect> ob_locations;
    std::vector<ExplosionCount> ob_explosions;

public:
    Obstacle(int num, const Rect& startbox, const Rect& endbox, const vec2<int>& revive,
             bool moveplayer, int addlives, const std::string& changemusic,
             const std::vector<Rect>& locations,
             const std::vector<ExplosionCount>& explosions) :
        ob_num(num),
        ob_startbox(startbox),
        ob_endbox(endbox),
        ob_revive(revive),
        action_moveplayer(moveplayer),
        action_addlives(addlives),
        action_changemusic(changemusic),
        ob_locations(locations),
        ob_explosions(explosions) {
            //
    }
    ~Obstacle() {
        //
    }
    void check_for_ob_start(const vec2<int>& player_pos);
    void check_for_ob_end(const vec2<int>& player_pos);
    void add_location();
    void add_event_explode_locs();
    void add_event_playsound();
    void add_event_teleport();
    void tick();
    void draw(const vec2<int>& offset) {
        // startbox
        Rect my_rect = ob_startbox;
        my_rect.position -= offset;
        draw_rect(my_rect, OB_STARTBOX_COL, true);
        
        // endbox
        my_rect = ob_endbox;
        my_rect.position -= offset;
        draw_rect(my_rect, OB_ENDBOX_COL, true);
    }
};
