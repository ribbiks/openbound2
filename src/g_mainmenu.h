#pragma once
#include <string>
#include <vector>

#include "g_gamestate.h"

class G_MainMenu : public GameState {
private:
    std::string map_folder;
    std::vector<std::string> map_list;

public:
    G_MainMenu();
    ~G_MainMenu() override;
    void update(Game* game, PlayerInputs* inputs) override;
    std::vector<Event> tick(Game* game) override;
    void draw(Game* game) override;
};
