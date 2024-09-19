#include "g_game.h"
#include "g_gamestate.h"
#include "g_mainmenu.h"
#include "g_bounding.h"

#include "globals.h"

Game::Game() : current_state(new G_MainMenu()) {
    camera = new Camera();
    camera->set_bounds({0, RESOLUTION.x}, {0, RESOLUTION.y});
}

void Game::change_state(std::unique_ptr<GameState> new_state) {
    current_state = std::move(new_state);
}

void Game::update(player_inputs* inputs, double frame_time) {
    camera->nudge_target(inputs->move_up, inputs->move_down, inputs->move_left, inputs->move_right, frame_time);
    camera->update(frame_time);
    current_state->update(this, inputs, frame_time);
}

void Game::tick(player_inputs* inputs){
    current_state->tick(this, inputs);
}

void Game::draw() {
    current_state->draw(this);
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

void Game::reset_camera_pos(const vec2<float>& target_pos) {
    if (camera != nullptr)
        camera->reset(target_pos);
}

void Game::set_camera_bounds(const vec2<int>& xrange, const vec2<int>& yrange) {
    if (camera != nullptr)
        camera->set_bounds(xrange, yrange);
}

Game::~Game() {
    delete camera;
    camera = nullptr;
}
