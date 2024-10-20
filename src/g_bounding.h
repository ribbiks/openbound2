#pragma once

#include "g_gamestate.h"

#include "geometry.h"
#include "Mauzling.h"
#include "WorldMap.h"

// if size of selection box is smaller than this, interpret it as a single click
static const int SELECTION_BOX_AS_CLICK = 16;

class G_Bounding : public GameState {
private:
    WorldMap* world_map = nullptr;
    Mauzling* player = nullptr;
    bool drawing_box = false;
    Rect selection_box = {{0,0}, {0,0}};
    bool rightmouse_was_up = true;
    PlayerOrder* most_recent_order = nullptr;
    int ingame_ticks = 0;

public:
    G_Bounding(Game* game, const std::string& map_filename);
    ~G_Bounding() override;
    void update(Game* game, PlayerInputs* inputs) override;
    std::vector<Event> tick(Game* game) override;
    void draw(Game* game) override;
};
