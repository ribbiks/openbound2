#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <fmt/format.h>

#include "Camera.h"
#include "Font.h"
#include "globals.h"
#include "inputs.h"
#include "Mauzling.h"
#include "misc_gfx.h"
#include "Vec2.h"
#include "WorldMap.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool quit = false;

Font* fps_font = nullptr;
Uint32 frame_count, frame_time, prev_time = 0;
float fps = 0.0f;
double current_time = 0.0;
double accumulator = 0.0;
double previous_update_time = 0.0;
int current_tic = 0;

player_inputs* inputs = nullptr;
player_inputs* previous_inputs = nullptr;
Camera* camera = nullptr;
WorldMap* world_map = nullptr;
Mauzling* player = nullptr;

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
    quit = inputs->quit;
    //
    camera->nudge_target(inputs->move_up, inputs->move_down, inputs->move_left, inputs->move_right, frame_time);
    vec2<int> camera_pos = camera->get_pos();
    vec2<int> camera_tgt = camera->get_target();
    camera->update(frame_time);

    // updates that occur every game tick
    while (accumulator >= DT) {
        //
        world_map->update();
        player->update();
        //
        delete previous_inputs;
        previous_inputs = new player_inputs(*inputs);
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
    world_map->draw(camera_pos);
    player->draw(camera_pos);
    //
    draw_text(fmt::format("FPS: {:.2f}", fps), 10, 10, fps_font);
    draw_text(fmt::format("{},{}", inputs->mouse_x, inputs->mouse_y), 10, 30, fps_font);
    draw_text(fmt::format("{}", current_tic), 10, 50, fps_font);
    draw_text(fmt::format("{},{} {},{}", camera_pos.x, camera_pos.y, camera_tgt.x, camera_tgt.y), 10, 70, fps_font);
    //
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    bool vsync = false;

    SDL_Color fps_text_color = {255, 255, 255, 255};
    fps_font = new Font("assets/small_font.png", fps_text_color, 2);

    inputs = new player_inputs;
    camera = new Camera();
    world_map = new WorldMap("maps/blah.json");
    //
    vec2<int> mapsize = world_map->get_map_size();
    camera->set_bounds({0, mapsize.x}, {0, mapsize.y});
    //
    player = new Mauzling(world_map->get_start_pos(), "assets/sq16.png");

    ///////////////////////////////////
    #ifdef __EMSCRIPTEN__ /////////////
    ///////////////////////////////////
    SDL_CreateWindowAndRenderer(RESOLUTION.x, RESOLUTION.y, 0, &window, &renderer);
    emscripten_set_main_loop(main_loop, 0, 1);
    //
    #else /////////////////////////////
    //
    window = SDL_CreateWindow("OpenBound v0.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, RESOLUTION.x, RESOLUTION.y, 0);
    if (vsync)
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    else
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    while (!quit) {
        main_loop();
    }
    ///////////////////////////////////
    #endif ////////////////////////////
    ///////////////////////////////////

    delete fps_font;
    fps_font = nullptr;

    delete inputs;
    delete camera;
    delete world_map;
    delete player;
    inputs = nullptr;
    camera = nullptr;
    world_map = nullptr;
    player = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
