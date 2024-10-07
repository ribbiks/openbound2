#include "AnimationManager.h"

#include <stdexcept>

#include "misc_gfx.h"

AnimationManager::AnimationManager() : all_animations(), active_animations() {}

AnimationManager::~AnimationManager() {
    for (auto& pair : all_animations) {
        for (size_t i = 0; i < pair.second.frames.size(); ++i)
            SDL_DestroyTexture(pair.second.frames[i]);
    }
}

void AnimationManager::add_animation(const std::string& name,
                                     const std::string& image_filename,
                                     const vec2<int>& sprite_dimensions,
                                     const std::vector<int>& frames_per_image) {
    std::vector<SDL_Surface*> image_list = load_spritesheet(image_filename, sprite_dimensions);
    if (frames_per_image.size() > 0 && frames_per_image.size() != image_list.size())
        throw std::invalid_argument("image_list size does not match frames_per_image size");
    AnimationSequence anim_dat;
    for (size_t i = 0; i < image_list.size(); ++i) {
        anim_dat.frames.push_back(SDL_CreateTextureFromSurface(renderer, image_list[i]));
        anim_dat.sizes.push_back({image_list[i]->w, image_list[i]->h});
        if (frames_per_image.size() > 0)
            anim_dat.durations.push_back(frames_per_image[i]);
        else
            anim_dat.durations.push_back(1);
        anim_dat.offsets.push_back({0,0}); // TODO: add support for offsets
        SDL_FreeSurface(image_list[i]);
    }
    all_animations[name] = anim_dat;
}

void AnimationManager::start_new_animation(const std::string& name, const std::string& id, const vec2<int>& position, bool is_looping) {
    if (all_animations.count(name) == 0)
        throw std::invalid_argument("all_animations does not contain image with name " + name);
    active_animations[id] = {name, position, 0, 0, is_looping};
}

SDL_Texture* AnimationManager::get_animating_texture(const std::string& id) {
    if (active_animations.count(id) == 0)
        return nullptr;
    AnimationSequence* my_animdat = &all_animations[active_animations[id].name];
    unsigned int current_frame = active_animations[id].current_frame;
    return my_animdat->frames[current_frame];
}

void AnimationManager::remove_animation(const std::string& id) {
    active_animations.erase(id);
}

void AnimationManager::remove_all_animations() {
    active_animations.clear();
}

void AnimationManager::tick() {
    for (auto it = active_animations.begin(); it != active_animations.end();) {
        AnimationSequence* my_animdat = &all_animations[it->second.name];
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
        AnimationSequence* my_animdat = &all_animations[pair.second.name];
        unsigned int current_frame = pair.second.current_frame;
        vec2<int> position = pair.second.position;
        SDL_Rect rect = {position.x + my_animdat->offsets[current_frame].x - offset.x,
                         position.y + my_animdat->offsets[current_frame].y - offset.y,
                         my_animdat->offsets[current_frame].x,
                         my_animdat->offsets[current_frame].y};
        SDL_RenderCopy(renderer, my_animdat->frames[current_frame], nullptr, &rect);
    }
}
