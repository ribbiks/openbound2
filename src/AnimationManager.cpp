#include "AnimationManager.h"

#include <stdexcept>

#include "misc_gfx.h"

AnimationManager::AnimationManager() : all_animations(), active_animations() {}

AnimationManager::~AnimationManager() {
    for (auto& pair : all_animations) {
    	for (size_t i = 0; i < pair.second.frames.size(); ++i)
    		SDL_FreeSurface(pair.second.frames[i]);
	}
}

void AnimationManager::add_animation(std::string name, std::vector<std::string> image_list, std::vector<int> frames_per_image) {
	if (frames_per_image.size() > 0 && frames_per_image.size() != image_list.size())
        throw std::invalid_argument("image_list size does not match frames_per_image size");
	animation_sequence anim_dat;
	for (size_t i = 0; i < image_list.size(); ++i) {
		anim_dat.frames.push_back(load_image(image_list[i]));
		if (frames_per_image.size() > 0)
			anim_dat.durations.push_back(frames_per_image[i]);
		else
			anim_dat.durations.push_back(1);
		anim_dat.offsets.push_back({0,0}); // TODO: add support for offsets
	}
	all_animations[name] = anim_dat;
}

void AnimationManager::start_new_animation(std::string name, std::string id, vec2<int> position, bool is_looping) {
    if (all_animations.count(name) == 0)
        throw std::invalid_argument("all_animations does not contain image with name " + name);
	active_animations[id] = {name, position, 0, 0, is_looping};
}

void AnimationManager::remove_animation(std::string id) {
	active_animations.erase(id);
}

void AnimationManager::remove_all_animations() {
	active_animations.clear();
}

void AnimationManager::tick() {
    for (auto it = active_animations.begin(); it != active_animations.end();) {
        animation_sequence* my_animdat = &all_animations[it->second.name];
        // increment animations ticks
        it->second.current_tick_within_frame += 1;
        //printf("frame %i: %i / %i\n", it->second.current_frame, it->second.current_tick_within_frame, my_animdat->durations[it->second.current_frame]);
        if (it->second.current_tick_within_frame > my_animdat->durations[it->second.current_frame]) {
            it->second.current_frame += 1;
            it->second.current_tick_within_frame = 0;
        }
        // animation finished: reset if looping, otherwise remove
        if (it->second.current_frame >= my_animdat->frames.size()) {
            if (it->second.is_looping) {
                it->second.current_frame = 0;
                it->second.current_tick_within_frame = 0;
                ++it;
            }
            else
                it = active_animations.erase(it);
        }
        else
            ++it;
    }
}

void AnimationManager::draw(const vec2<int>& offset) {
    for (const auto& pair : active_animations) {
        animation_sequence* my_animdat = &all_animations[pair.second.name];
        unsigned int current_frame = pair.second.current_frame;
        vec2<int> position = pair.second.position;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, my_animdat->frames[current_frame]);
        if (texture == nullptr) {
            SDL_Log("Unable to create texture! SDL Error: %s\n", SDL_GetError());
            return;
        }
        // fiddle with these numbers until it's aligned properly
        SDL_Rect rect = {position.x + my_animdat->offsets[current_frame].x - offset.x,
                         position.y + my_animdat->offsets[current_frame].y - offset.y,
                         my_animdat->frames[current_frame]->w,
                         my_animdat->frames[current_frame]->h};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }
}
