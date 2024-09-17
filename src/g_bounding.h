#pragma once

#include "g_gamestate.h"

#include "Mauzling.h"
#include "WorldMap.h"
#include "Vec2.h"

class G_Bounding : public GameState {
private:
    WorldMap* world_map = nullptr;
    Mauzling* player = nullptr;

public:
    G_Bounding(Game* game, const std::string& map_filename);
    ~G_Bounding() override;
    void update(Game* game, player_inputs* inputs, double frame_time) override;
    void tick(Game* game) override;
    void draw(Game* game) override;
};
