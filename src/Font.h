#pragma once

#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const SDL_Color LETTER_COL = {  0,   0,   0, 255};
const SDL_Color DELIM_COL  = {127, 127, 127, 255};
const SDL_Color TRANS_COL  = {255,   0, 255, 255};

class Font {
private:
    int spacing;
    int char_height;
    std::unordered_map<char, SDL_Surface*> characters;
    std::unordered_map<char, int> char_width;

public:
    Font(const std::string& path, SDL_Color color, int scalar = 1)
        : spacing(scalar), char_height(0) {
        const std::string character_order = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.-,:+\'!?0123456789()/_=\\[]*\"<>>;|";

        SDL_Surface* font_img = IMG_Load(path.c_str());
        if (!font_img) {
            const char* error = IMG_GetError();
            printf("Failed to load font image: %s %s\n", path.c_str(), error);
            return;
        }

        // recolor
        SDL_LockSurface(font_img);
        Uint32* pixels = static_cast<Uint32*>(font_img->pixels);
        for (int y = 0; y < font_img->h; ++y) {
            for (int x = 0; x < font_img->w; ++x) {
                Uint32 pixel = pixels[y * font_img->w + x];
                Uint8 r, g, b, a;
                SDL_GetRGBA(pixel, font_img->format, &r, &g, &b, &a);
                if (r == LETTER_COL.r && g == LETTER_COL.g && b == LETTER_COL.b && a == 255) {
                    pixels[y * font_img->w + x] = SDL_MapRGBA(font_img->format, color.r, color.g, color.b, 255);
                }
            }
        }
        SDL_UnlockSurface(font_img);

        // separate characters
        int current_char_width = 0;
        int character_count = 0;
        for (int x = 0; x < font_img->w; ++x) {
            Uint32 pixel = pixels[x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, font_img->format, &r, &g, &b, &a);

            if (r == DELIM_COL.r && g == DELIM_COL.g && b == DELIM_COL.b) {
                SDL_Rect char_rect = {x - current_char_width, 0, current_char_width, font_img->h};
                SDL_Surface* char_surface = SDL_CreateRGBSurface(0, char_rect.w, char_rect.h, 32, 0, 0, 0, 0);
                SDL_BlitSurface(font_img, &char_rect, char_surface, nullptr);

                if (scalar > 1) {
                    SDL_Surface* scaled_surface = SDL_CreateRGBSurface(0, char_rect.w * scalar, char_rect.h * scalar, 32, 0, 0, 0, 0);
                    SDL_BlitScaled(char_surface, nullptr, scaled_surface, nullptr);
                    SDL_FreeSurface(char_surface);
                    char_surface = scaled_surface;
                }

                char c = character_order[character_count];
                characters[c] = char_surface;
                char_width[c] = char_surface->w;

                character_count++;
                current_char_width = 0;
            } else {
                current_char_width++;
            }
        }

        char_height = characters['A']->h;
        char_width[' '] = char_width['A'];

        SDL_FreeSurface(font_img);
    }

    ~Font() {
        for (auto& pair : characters) {
            SDL_FreeSurface(pair.second);
        }
    }

    SDL_Surface* render_text(const std::string& text) {
        if (text.empty()) {
            return nullptr;
        }

        // calculate the total width of the text surface
        int total_width = 0;
        for (char c : text) {
            if (char_width.find(c) != char_width.end()) {
                total_width += char_width[c] + spacing;
            } else if (c == ' ') {
                total_width += char_width[' '] + spacing;
            }
        }
        total_width -= spacing; // remove extra spacing after the last character

        // create a surface to hold the entire text
        SDL_Surface* text_surface = SDL_CreateRGBSurface(0, total_width, char_height, 32, 0, 0, 0, 0);
        if (!text_surface) {
            return nullptr;
        }
        SDL_Rect floodfill = {0, 0, text_surface->w, text_surface->h};
        SDL_FillRect(text_surface, &floodfill, SDL_MapRGB(text_surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));
        SDL_SetColorKey(text_surface, SDL_TRUE, SDL_MapRGB(text_surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));

        int current_x = 0;
        for (char c : text) {
            if (characters.find(c) != characters.end()) {
                SDL_Rect dst_rect = {current_x, 0, char_width[c], char_height};
                SDL_BlitSurface(characters[c], nullptr, text_surface, &dst_rect);
                current_x += char_width[c] + spacing;
            } else if (c == ' ') {
                current_x += char_width[' '] + spacing;
            }
        }

        return text_surface;
    }
};
