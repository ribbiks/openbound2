#include "g_game.h"
#include "g_gamestate.h"
#include "g_mainmenu.h"
#include "g_bounding.h"

#include "globals.h"

Game::Game() : current_state(new G_MainMenu()) {
    camera = new Camera();
    camera->set_bounds({0, RESOLUTION.x}, {0, RESOLUTION.y});
    cursor = new Cursor({"assets/cursor_0.png",
                         "assets/cursor_1.png",
                         "assets/cursor_2.png",
                         "assets/cursor_3.png",
                         "assets/cursor_4.png"},
                         "assets/cursor_click.png",
                         "assets/cursor_shiftclick.png");
    animation_manager.add_animation("small_blue", "assets/M484explosionset1.png", {34,34});
}

Game::~Game() {
    delete camera;
    delete cursor;
    camera = nullptr;
    cursor = nullptr;
}

void Game::change_state(std::unique_ptr<GameState> new_state) {
    current_state = std::move(new_state);
}

void Game::update(PlayerInputs* inputs, double frame_time) {
    camera->nudge_target(inputs->move_up, inputs->move_down, inputs->move_left, inputs->move_right, frame_time);
    camera->update(frame_time);
    cursor->update({inputs->mouse_x, inputs->mouse_y});
    current_state->update(this, inputs);
}

void Game::tick(){
    cursor->tick();
    std::vector<Event> state_events = current_state->tick(this);
    for (auto event : state_events){
        //printf("RAWR %s \n", event.type.c_str());
        if (event.type == "start_animation") {
            animation_manager.start_new_animation(event.data_str , event.data_str_2, event.data_vec, false, true);
        }
    }
    animation_manager.tick();
}

void Game::draw() {
    current_state->draw(this);
    cursor->draw();
}

vec2<int> Game::get_camera_pos() {
    if (camera != nullptr)
        return camera->get_pos();
    return {0,0};
}

vec2<int> Game::get_camera_target() {
    if (camera != nullptr)
        return camera->get_target();
    return {0,0};
}

vec2<int> Game::get_cursor_pos() {
    if (cursor != nullptr)
        return cursor->get_pos();
    return {0,0};
}

void Game::cursor_click_animation(bool is_queue) {
    if (cursor != nullptr)
        cursor->start_click_animation(is_queue);
}

void Game::reset_camera_pos(const vec2<float>& target_pos) {
    if (camera != nullptr)
        camera->reset(target_pos);
}

void Game::set_camera_bounds(const vec2<int>& xrange, const vec2<int>& yrange) {
    if (camera != nullptr)
        camera->set_bounds(xrange, yrange);
}
