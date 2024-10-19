#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL.h>

#include "Vec2.h"

extern SDL_Renderer* renderer;

struct AnimationSequence {
    std::vector<SDL_Texture*> frames;
    std::vector<vec2<int>> sizes;
    std::vector<unsigned int> durations;
    std::vector<vec2<int>> offsets;
};

struct ActiveAnimation {
    std::string name;
    vec2<int> position;
    unsigned int current_frame;
    unsigned int current_tick_within_frame;
    bool is_looping;
    bool is_centered;
};

class AnimationManager {
private:
     std::unordered_map<std::string, AnimationSequence> all_animations;
     std::unordered_map<std::string, ActiveAnimation> active_animations;

public:
    AnimationManager();
    ~AnimationManager();
    void add_animation(const std::string& name, const std::string& image_list, const vec2<int>& sprite_dimensions, const std::vector<int>& frames_per_image = {});
    void start_new_animation(const std::string& name, const std::string& id, const vec2<int>& position, bool is_looping, bool is_centered = false);
    SDL_Texture* get_animating_texture(const std::string& id);
    void remove_animation(const std::string& id);
    void remove_all_animations();
    void tick();
    void draw(const vec2<int>& offset);
};
