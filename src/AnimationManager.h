#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL.h>

#include "Vec2.h"

extern SDL_Renderer* renderer;

struct animation_sequence {
    std::vector<SDL_Surface*> frames;
    std::vector<unsigned int> durations;
    std::vector<vec2<int>> offsets;
};

struct active_animation {
    std::string name;
	vec2<int> position;
	unsigned int current_frame;
	unsigned int current_tick_within_frame;
	bool is_looping;
};

class AnimationManager {
private:
     std::unordered_map<std::string, animation_sequence> all_animations;
     std::unordered_map<std::string, active_animation> active_animations;

public:
    AnimationManager();
    ~AnimationManager();
    void add_animation(std::string name, std::vector<std::string> image_list, std::vector<int> frames_per_image = {});
    void start_new_animation(std::string name, std::string id, vec2<int> position, bool is_looping);
    void remove_animation(std::string id);
    void remove_all_animations();
    void tick();
    void draw(const vec2<int>& offset);
};
