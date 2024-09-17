#include "g_bounding.h"
#include "g_game.h"

#include "Vec2.h"

G_Bounding::G_Bounding(Game* game, const std::string& map_filename) {
    world_map = new WorldMap(map_filename);
    vec2<int> mapsize = world_map->get_map_size();
    player = new Mauzling(world_map->get_start_pos(), "assets/sq16.png");
    //
    game->reset_camera_pos({0,0}); // change to start player coordinates?
    game->set_camera_bounds({0, mapsize.x}, {0, mapsize.y});
}

G_Bounding::~G_Bounding() {
    delete world_map;
    delete player;
    world_map = nullptr;
    player = nullptr;
}

void G_Bounding::update(Game* game, player_inputs* inputs, double frame_time) {
    //
}

void G_Bounding::tick(Game* game) {
    world_map->update();
    player->update();
}

void G_Bounding::draw(Game* game) {
    vec2<int> camera_pos = game->get_camera_pos();
    world_map->draw(camera_pos);
    player->draw(camera_pos);
}
