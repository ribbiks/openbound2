#pragma once
#include <memory>

#include "Camera.h"
#include "inputs.h"
#include "Vec2.h"

class GameState;

class Game {
private:
    std::unique_ptr<GameState> current_state;
    Camera* camera = nullptr;

public:
    Game();
    void change_state(std::unique_ptr<GameState> new_state);
    void update(player_inputs* inputs, double frame_time);
    void tick(player_inputs* inputs);
    void draw();
    vec2<int> get_camera_pos();
    vec2<int> get_camera_target();
    void reset_camera_pos(const vec2<float>& target_pos);
    void set_camera_bounds(const vec2<int>& xrange, const vec2<int>& yrange);
    ~Game();
};
