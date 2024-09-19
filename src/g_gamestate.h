#pragma once

#include "inputs.h"

class Game;

class GameState {
public:
    virtual void update(Game* game, player_inputs* inputs, double frame_time) = 0;
    virtual void tick(Game* game, player_inputs* inputs) = 0;
    virtual void draw(Game* game) = 0;
    virtual ~GameState() = default;
};
