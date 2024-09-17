#pragma once

#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>

static const std::string character_order = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-,:+\'!?0123456789()/_=\\[]*\"<>>;|";

class Font {
private:
    int spacing;
    int char_height;
    std::unordered_map<char, SDL_Surface*> characters;
    std::unordered_map<char, int> char_width;

public:
    Font(const std::string& path, SDL_Color color, int scalar = 1);
    ~Font();
    SDL_Surface* render_text(const std::string& text);
};
