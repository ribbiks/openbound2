#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL.h>

#include "geometry.h"
#include "globals.h"
#include "misc_gfx.h"
#include "Vec2.h"

extern SDL_Renderer* renderer;
extern std::unordered_map<std::string, Font*> fonts;

static const int REVIVE_RADIUS = 4; // width of revive position (for drawing)

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

        // revive
        my_rect = {ob_revive - vec2<int>(REVIVE_RADIUS, REVIVE_RADIUS), {2 * REVIVE_RADIUS, 2 * REVIVE_RADIUS}};
        my_rect.position -= offset;
        draw_rect(my_rect, OB_REVIVE_COL, true);

        // locs
        for (size_t i = 0; i < ob_locations.size(); ++i) {
            std::string loc_string = std::to_string(ob_num) + "-" + std::to_string(i+1);
            my_rect = ob_locations[i];
            my_rect.position -= offset;
            draw_rect(my_rect, OB_LOC_COL, true);
            fonts["tiny_black"]->draw_text(loc_string, my_rect.position + vec2<int>(2,2));
        }
    }
};
