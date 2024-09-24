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
    cursor = new Cursor({"assets/cursor_0.png", "assets/cursor_1.png", "assets/cursor_2.png", "assets/cursor_3.png", "assets/cursor_4.png"},
                        "assets/cursor_click.png",
                        "assets/cursor_shiftclick.png");
    //
    game->reset_camera_pos({0,0}); // change to start player coordinates?
    game->set_camera_bounds({0, mapsize.x}, {0, mapsize.y});
}

G_Bounding::~G_Bounding() {
    delete world_map;
    delete player;
    delete cursor;
    delete most_recent_order;
    world_map = nullptr;
    player = nullptr;
    cursor = nullptr;
    most_recent_order = nullptr;
}

void G_Bounding::update(Game* game, player_inputs* inputs, double frame_time) {
    //
    // player selection and orders
    //
    vec2<int> camera_pos = game->get_camera_pos();
    mouse_pos_screen = {inputs->mouse_x, inputs->mouse_y};
    vec2<int> mouse_pos = mouse_pos_screen + camera_pos;
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

void G_Bounding::tick(Game* game, player_inputs* inputs) {
    vec2<int> camera_pos = game->get_camera_pos();
    //
    world_map->tick();
    cursor->tick();
    //
    // issue order
    //
    if (most_recent_order != nullptr) {
        bool animate_cursor = player->issue_new_order(most_recent_order->coordinates, most_recent_order->is_queue);
        delete most_recent_order;
        most_recent_order = nullptr;
    }
    player->tick();
}

void G_Bounding::draw(Game* game) {
    vec2<int> camera_pos = game->get_camera_pos();
    world_map->draw(camera_pos);
    player->draw(camera_pos);
    if (drawing_box) {
        Rect offset_box = {selection_box.position - camera_pos, selection_box.size};
        draw_rect(offset_box, SELECTION_BOX_COL);
    }
    cursor->draw(mouse_pos_screen.x, mouse_pos_screen.y);
}
