#include "Font.h"

#include <string>
#include <unordered_map>

#include "misc_gfx.h"
#include "globals.h"

Font::Font() : spacing(1), char_height(0) {}

Font::Font(const std::string& path, SDL_Color color, int scalar) :
    spacing(scalar),
    char_height(0) {

    SDL_Surface* font_img = load_image(path, false);

    // recolor
    SDL_LockSurface(font_img);
    Uint32* pixels = static_cast<Uint32*>(font_img->pixels);
    for (int y = 0; y < font_img->h; ++y) {
        for (int x = 0; x < font_img->w; ++x) {
            Uint32 pixel = pixels[y * font_img->w + x];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, font_img->format, &r, &g, &b, &a);
            if (r == FONT_LETTER_COL.r && g == FONT_LETTER_COL.g && b == FONT_LETTER_COL.b && a == 255) {
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

        if (r == FONT_DELIM_COL.r && g == FONT_DELIM_COL.g && b == FONT_DELIM_COL.b) {
            SDL_Rect char_rect = {x - current_char_width, 0, current_char_width, font_img->h};
            SDL_Surface* char_surface = SDL_CreateRGBSurface(0, char_rect.w, char_rect.h, 32, 0, 0, 0, 0);
            SDL_Rect floodfill = {0, 0, char_surface->w, char_surface->h};
            SDL_FillRect(char_surface, &floodfill, SDL_MapRGB(char_surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));
            SDL_BlitSurface(font_img, &char_rect, char_surface, nullptr);
            
            if (scalar > 1) {
                SDL_Surface* scaled_surface = SDL_CreateRGBSurface(0, char_rect.w * scalar, char_rect.h * scalar, 32, 0, 0, 0, 0);
                SDL_BlitScaled(char_surface, nullptr, scaled_surface, nullptr);
                SDL_FreeSurface(char_surface);
                char_surface = scaled_surface;
            }

            char c = CHARACTER_ORDER[character_count];
            char_width[c] = char_surface->w;
            char_height = std::max(char_height, char_surface->h);
            SDL_SetColorKey(char_surface, SDL_TRUE, SDL_MapRGB(char_surface->format, TRANS_COL.r, TRANS_COL.g, TRANS_COL.b));
            char_textures[c] = SDL_CreateTextureFromSurface(renderer, char_surface);
            SDL_FreeSurface(char_surface);

            character_count++;
            current_char_width = 0;
        } else {
            current_char_width++;
        }
    }
    SDL_FreeSurface(font_img);
    char_width[' '] = char_width['A'];
}

Font::~Font() {
    for (auto& pair : char_textures)
        SDL_DestroyTexture(pair.second);
}

void Font::draw_text(const std::string& text, const vec2<int>& position) {
    int current_x = 0;
    for (char c : text) {
        if (char_textures.find(c) != char_textures.end()) {
            SDL_Rect rect = {current_x + position.x, position.y, char_width[c], char_height};
            SDL_RenderCopy(renderer, char_textures[c], nullptr, &rect);
            current_x += char_width[c] + spacing;
        } else if (c == ' ') {
            current_x += char_width[' '] + spacing;
        }
    }
}
