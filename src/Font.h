#pragma once
#include <string>
#include <unordered_map>

#include <SDL.h>

#include "Vec2.h"

extern SDL_Renderer* renderer;

static const std::string CHARACTER_ORDER = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-,:+\'!?0123456789()/_=\\[]*\"<>>;|";

class Font {
private:
    int spacing;
    int char_height;
    std::unordered_map<char, SDL_Texture*> char_textures;
    std::unordered_map<char, int> char_width;

public:
    Font(const std::string& path, SDL_Color color, int scalar = 1);
    ~Font();
    void draw_text(const std::string& text, const vec2<int>& position);
};
