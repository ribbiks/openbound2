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
    int ob_timer;
    int ob_currentcount;
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
        if (ob_explosions.size() > 0) {
            ob_currentcount = ob_explosions.size() - 1;
            ob_timer = ob_explosions[ob_currentcount].delay;
        }
    }

    ~Obstacle() {
        //
    }

    void reset() {
        if (ob_explosions.size() > 0) {
            ob_currentcount = ob_explosions.size() - 1;
            ob_timer = ob_explosions[ob_currentcount].delay;
        }
    }

    void check_for_ob_start(const vec2<int>& player_pos);
    void check_for_ob_end(const vec2<int>& player_pos);
    void add_location();
    void add_event_explode_locs();
    void add_event_playsound();
    void add_event_teleport();

    std::vector<Event> tick() {
        std::vector<Event> out_events;
        int num_explosions = ob_explosions.size();
        if (num_explosions > 0) {
            ob_timer += 1;
            bool execute_explosion = false;
            if (ob_timer > ob_explosions[ob_currentcount].delay) {
                execute_explosion = true;
                ob_currentcount += 1;
                ob_timer = 0;
                if (ob_currentcount >= num_explosions)
                    ob_currentcount = 0;
            }
            //printf("tick: %i [%i]\n", ob_timer, ob_currentcount);
            if (execute_explosion) {
                printf("Bang [%i]!\n", ob_currentcount);
                for (size_t i = 0; i < ob_explosions[ob_currentcount].locs.size(); ++i) {
                    int loc_num = ob_explosions[ob_currentcount].locs[i];
                    Rect exp_loc = ob_locations[loc_num - 1];
                    Event new_event;
                    new_event.type = "start_animation";
                    new_event.data_str = ob_explosions[ob_currentcount].units[i]; // explosion sprite
                    new_event.data_str_2 = std::to_string(ob_num) + "-" + std::to_string(loc_num); // animation id: obnum_loc
                    new_event.data_vec = exp_loc.position + vec2<int>(exp_loc.size.x / 2, exp_loc.size.y / 2); // centered coords
                    out_events.push_back(new_event);
                }
            }
        }
        return out_events;
    }

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
