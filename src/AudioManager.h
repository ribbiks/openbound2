#pragma once
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <SDL_mixer.h>

class AudioManager {
private:
     std::unordered_map<std::string, Mix_Chunk*> all_sounds;
     Mix_Music* current_music;

public:
    AudioManager() : current_music(nullptr) {
        Mix_AllocateChannels(16);
    }

    ~AudioManager() {
        if (current_music)
            Mix_FreeMusic(current_music);
        for (const auto& pair : all_sounds)
            Mix_FreeChunk(pair.second);
    }

    void load_music(const std::string& path) {
        if (current_music)
            Mix_FreeMusic(current_music);
        current_music = Mix_LoadMUS(path.c_str());
        if (!current_music)
            throw std::runtime_error(Mix_GetError());
    }

    void play_music(int loops = -1) {
        // -1 --> loop forever
        //  0 --> play once
        if (Mix_PlayMusic(current_music, loops) == -1)
            throw std::runtime_error(Mix_GetError());
    }

    void pause_music() {
        Mix_PauseMusic();
    }
    
    void resume_music() {
        Mix_ResumeMusic();
    }
    
    void stop_music() {
        Mix_HaltMusic();
    }
    
    void fadeout_music(int ms) {
        Mix_FadeOutMusic(ms);
    }

    void load_sound(const std::string& name, const std::string& path) {
        Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
        if (!chunk)
            throw std::runtime_error(Mix_GetError());
        all_sounds[name] = chunk;
    }

    void play_sound(const std::string& name, float volume = 1.0f) {
        if (all_sounds.count(name) == 0)
            throw std::invalid_argument("all_sounds does not contain sound with name " + name);
        int channel = Mix_PlayChannel(-1, all_sounds[name], 0);
        if (channel != -1) {
            int vol = std::min(128, std::max(0, static_cast<int>(128 * volume)));
            Mix_Volume(channel, vol);
        }
    }

    void stop_all_sounds() {
        Mix_HaltChannel(-1);
    }
};
