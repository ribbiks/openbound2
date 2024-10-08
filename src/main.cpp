#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <memory>
#include <SDL.h>
#include <fmt/format.h>

#include "Font.h"
#include "globals.h"
#include "g_game.h"
#include "inputs.h"
#include "misc_gfx.h"
#include "Vec2.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool quit = false;
std::unique_ptr<Game> game;
Font* fps_font = nullptr;
PlayerInputs* inputs = nullptr;

Uint32 frame_count, frame_time, prev_time = 0;
float fps = 0.0f;
double current_time = 0.0;
double accumulator = 0.0;
double previous_update_time = 0.0;
int current_tic = 0;

void set_game_globals() {
    game = std::unique_ptr<Game>(new Game());
    fps_font = new Font("assets/small_font.png", {255, 255, 255, 255}, 2);
    inputs = new PlayerInputs;
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

    // DEBUGGING STUFF
    //if (current_tic == 200) {
    //    game->animation_manager->add_animation("test_explosion", "assets/exp2_0.png", {64,64});
    //    game->animation_manager->start_new_animation("test_explosion", "anim_id_1", {256,256}, true);
    //}

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
        game->tick(inputs);
        //
        previous_update_time = current_time;
        accumulator -= DT;
        current_tic++;
    }

    // drawing
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    //
    draw_background_grid(camera_pos);
    //
    game->draw();
    //
    fps_font->draw_text(fmt::format("FPS: {:.2f}", fps), {10, 10});
    fps_font->draw_text(fmt::format("{},{}", inputs->mouse_x / GRIDSIZE, inputs->mouse_y / GRIDSIZE), {10, 30});
    fps_font->draw_text(fmt::format("{}", current_tic), {10, 50});
    fps_font->draw_text(fmt::format("{},{} {},{}", camera_pos.x, camera_pos.y, camera_tgt.x, camera_tgt.y), {10, 70});
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

    delete fps_font;
    fps_font = nullptr;

    delete inputs;
    inputs = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
