#pragma once
#include <vector>

#include "globals.h"
#include "inputs.h"

class Game;

class GameState {
public:
    virtual void update(Game* game, PlayerInputs* inputs) = 0;
    virtual std::vector<Event> tick(Game* game) = 0;
    virtual void draw(Game* game) = 0;
    virtual ~GameState() = default;
};
