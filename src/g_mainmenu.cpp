#include "g_mainmenu.h"
#include "g_game.h"
#include "g_bounding.h"

G_MainMenu::G_MainMenu() {
    map_list.push_back("blah.json");
}

G_MainMenu::~G_MainMenu() {
    //
}

void G_MainMenu::update(Game* game, PlayerInputs* inputs) {
    // if not emscripten, escape triggers quit
    #ifndef __EMSCRIPTEN__
    if (inputs->key_escape) {
        inputs->quit = true;
        return;
    }
    #endif
    //
    if (inputs->key_enter) {
        game->change_state(std::unique_ptr<GameState>(new G_Bounding(game, "maps/blah.json")));
    }
}

std::vector<Event> G_MainMenu::tick(Game* game) {
    std::vector<Event> out_events;
    return out_events;
}

void G_MainMenu::draw(Game* game) {
    //
}
