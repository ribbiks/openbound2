#include "g_bounding.h"
#include "g_game.h"

#include "globals.h"
#include "misc_gfx.h"
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
    delete most_recent_order;
    world_map = nullptr;
    player = nullptr;
    most_recent_order = nullptr;
}

void G_Bounding::update(Game* game, PlayerInputs* inputs) {
    //
    // player selection and orders
    //
    vec2<int> camera_pos = game->get_camera_pos();
    vec2<int> mouse_pos = game->get_cursor_pos() + camera_pos;
    if (!drawing_box && inputs->leftmouse_down) {
        selection_box.position = mouse_pos;
        drawing_box = true;
    }
    if (drawing_box)
        selection_box.size = mouse_pos - selection_box.position;
    if (inputs->leftmouse_up) {
        if (drawing_box) {
            int boxsize = std::abs(selection_box.size.x) + std::abs(selection_box.size.y);
            vec2<int> release_pos = selection_box.position + selection_box.size;
            if (boxsize <= SELECTION_BOX_AS_CLICK)
                player->check_selection_click(release_pos);
            else
                player->check_selection_box(selection_box.position, release_pos);
        }
        drawing_box = false;
    }
    if (inputs->rightmouse_up)
        rightmouse_was_up = true;
    if (inputs->rightmouse_down && rightmouse_was_up) {
        most_recent_order = new PlayerOrder();
        most_recent_order->coordinates = mouse_pos;
        most_recent_order->is_queue = inputs->key_shift;
        rightmouse_was_up = false;
    }
}

void G_Bounding::tick(Game* game, PlayerInputs* inputs) {
    //
    // DEBUG / TESTING STUFF
    //
    if (ingame_ticks % 200 == 0) {
        std::vector<vec2<int>> test_coords;
        std::vector<int> test_tiles;
        test_coords.push_back({25,28});
        test_coords.push_back({26,28});
        test_coords.push_back({25,29});
        test_coords.push_back({26,29});
        test_tiles.push_back(0);
        test_tiles.push_back(0);
        test_tiles.push_back(0);
        test_tiles.push_back(0);
        world_map->change_map_tiles(test_coords, test_tiles);
        world_map->set_current_obstacle(0);
    }
    if ((ingame_ticks + 100) % 200 == 0) {
        std::vector<vec2<int>> test_coords;
        std::vector<int> test_tiles;
        test_coords.push_back({25,28});
        test_coords.push_back({26,28});
        test_coords.push_back({25,29});
        test_coords.push_back({26,29});
        test_tiles.push_back(1);
        test_tiles.push_back(1);
        test_tiles.push_back(1);
        test_tiles.push_back(1);
        world_map->change_map_tiles(test_coords, test_tiles);
        world_map->set_current_obstacle(-1);
    }
    //
    world_map->tick();
    //
    // issue order
    //
    if (most_recent_order != nullptr) {
        bool animate_cursor = player->issue_new_order(most_recent_order->coordinates, most_recent_order->is_queue);
        if (animate_cursor)
            game->cursor_click_animation(most_recent_order->is_queue);
        delete most_recent_order;
        most_recent_order = nullptr;
    }
    player->tick(world_map);
    //
    ingame_ticks += 1;
}

void G_Bounding::draw(Game* game) {
    vec2<int> camera_pos = game->get_camera_pos();
    world_map->draw(camera_pos);
    player->draw(camera_pos);
    game->animation_manager->draw(camera_pos); // draw sprites on top of player, but not on top of selection box
    if (drawing_box) {
        Rect offset_box = {selection_box.position - camera_pos, selection_box.size};
        //draw_rect(offset_box, SELECTION_BOX_COL);
        draw_thick_rect(offset_box, SELECTION_BOX_COL, 2);
    }
}
