#pragma once
#include <memory>

#include "AnimationManager.h"
#include "Camera.h"
#include "Cursor.h"
#include "inputs.h"
#include "Vec2.h"

class GameState;

class Game {
private:
    std::unique_ptr<GameState> current_state;
    Camera* camera = nullptr;
    Cursor* cursor = nullptr;

public:
    AnimationManager* animation_manager = nullptr;
    
    Game();
    ~Game();
    void change_state(std::unique_ptr<GameState> new_state);
    void update(player_inputs* inputs, double frame_time);
    void tick(player_inputs* inputs);
    void draw();
    vec2<int> get_camera_pos();
    vec2<int> get_camera_target();
    vec2<int> get_cursor_pos();
    void reset_camera_pos(const vec2<float>& target_pos);
    void set_camera_bounds(const vec2<int>& xrange, const vec2<int>& yrange);
};
