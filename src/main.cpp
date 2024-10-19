#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <memory>
#include <string>
#include <unordered_map>

#include <fmt/format.h>
#include <SDL.h>

#include "Font.h"
#include "globals.h"
#include "g_game.h"
#include "inputs.h"
#include "misc_gfx.h"
#include "Vec2.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
PlayerInputs* inputs = nullptr;
std::unique_ptr<Game> game;
bool quit = false;

std::unordered_map<std::string, Font*> fonts;

Uint32 frame_count = 0;
Uint32 frame_time = 0;
Uint32 prev_time = 0;
float fps = 0.0f;
double current_time = 0.0;
double accumulator = 0.0;
double previous_update_time = 0.0;
int current_tic = 0;

void set_game_globals() {
    game = std::unique_ptr<Game>(new Game());
    inputs = new PlayerInputs;
    inputs->key_space = false;
    inputs->key_enter = false;
    inputs->key_shift = false;
    inputs->key_escape = false;
    fonts["tiny_white"]  = new Font("assets/small_font.png", {255, 255, 255, 255}, 1);
    fonts["small_white"] = new Font("assets/small_font.png", {255, 255, 255, 255}, 2);
    fonts["tiny_black"]  = new Font("assets/small_font.png", {  0,   0,   0, 255}, 1);
    fonts["small_black"] = new Font("assets/small_font.png", {  0,   0,   0, 255}, 2);
}

void clear_game_globals() {
    delete inputs;
    inputs = nullptr;
    for (auto& pair : fonts) {
        delete pair.second;
        pair.second = nullptr;
    }
    fonts.clear();
}

void update_fps() {
    frame_count++;
    frame_time = SDL_GetTicks() - prev_time;
    if (frame_time >= 1000) {
        fps = frame_count / (frame_time / 1000.0f);
        frame_count = 0;
        prev_time = SDL_GetTicks();
    }
}

void main_loop() {
    double new_time = SDL_GetTicks() / 1000.0;
    double frame_time = new_time - current_time;
    current_time = new_time;
    accumulator = std::min(accumulator + frame_time, MAX_ACCUM);

    // updates that occur every frame (camera, mouse cursors)
    update_fps();
    get_inputs(inputs);
    game->update(inputs, frame_time);
    quit = inputs->quit;
    //
    vec2<int> camera_pos = game->get_camera_pos();
    vec2<int> camera_tgt = game->get_camera_target();

    // updates that occur every game tick
    while (accumulator >= DT) {
        //
        game->tick();
        //
        previous_update_time = current_time;
        accumulator -= DT;
        current_tic++;
        //
        // DEBUGGING STUFF
        ////if (current_tic == 100) {
        ////    game->animation_manager.add_animation("test", "assets/M484explosionset1.png", {34,34});
        ////    game->animation_manager.start_new_animation("test", "anim_id_1", {128,384}, true);
        ////}
    }

    // drawing
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    //
    draw_background_grid(camera_pos);
    //
    game->draw();
    //
    fonts["small_white"]->draw_text(fmt::format("FPS: {:.2f}", fps), {10, 10});
    fonts["small_white"]->draw_text(fmt::format("{},{} ({},{})", inputs->mouse_x, inputs->mouse_y, inputs->mouse_x / GRIDSIZE, inputs->mouse_y / GRIDSIZE), {10, 30});
    fonts["small_white"]->draw_text(fmt::format("{}", current_tic), {10, 50});
    fonts["small_white"]->draw_text(fmt::format("{},{} {},{}", camera_pos.x, camera_pos.y, camera_tgt.x, camera_tgt.y), {10, 70});
    //
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    bool vsync = false;

    ///////////////////////////////////
    #ifdef __EMSCRIPTEN__ /////////////
    ///////////////////////////////////
    SDL_CreateWindowAndRenderer(RESOLUTION.x, RESOLUTION.y, 0, &window, &renderer);
    set_game_globals();
    emscripten_set_main_loop(main_loop, 0, 1);
    //
    #else /////////////////////////////
    //
    window = SDL_CreateWindow("OpenBound v0.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, RESOLUTION.x, RESOLUTION.y, 0);
    if (vsync)
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    else
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    set_game_globals();
    while (!quit) {
        main_loop();
    }
    ///////////////////////////////////
    #endif ////////////////////////////
    ///////////////////////////////////

    clear_game_globals();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
