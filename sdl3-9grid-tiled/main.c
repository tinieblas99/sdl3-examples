#include <math.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

enum {
    WINDOW_WIDTH = 900,
    WINDOW_HEIGHT = 620,
    CORNER_SIZE = 24
};

static void render_panel(SDL_Renderer *renderer, SDL_Texture *texture,
                         float x, float y, float w, float h,
                         float corner_scale, float tile_scale)
{
    SDL_FRect dst = {   
        SDL_roundf(x),   
        SDL_roundf(y),   
        SDL_roundf(w),   
        SDL_roundf(h)   
    };
    SDL_RenderTexture9Grid(renderer, texture, NULL,
                                CORNER_SIZE, CORNER_SIZE,
                                CORNER_SIZE, CORNER_SIZE,
                                corner_scale, &dst);
}

static void render_panel_tiled(SDL_Renderer *renderer, SDL_Texture *texture,
                         float x, float y, float w, float h,
                         float corner_scale, float tile_scale)
{
    SDL_FRect dst = {   
        SDL_roundf(x),   
        SDL_roundf(y),   
        SDL_roundf(w),   
        SDL_roundf(h)   
    };
    SDL_RenderTexture9GridTiled(renderer, texture, NULL,
                                CORNER_SIZE, CORNER_SIZE,
                                CORNER_SIZE, CORNER_SIZE,
                                corner_scale, &dst, tile_scale);     
}

int main(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL3 9-Grid Tiled Gothic Panel",
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!window || !renderer) {
        SDL_Log("Window/renderer creation failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *panel = IMG_LoadTexture(renderer, "assets/gothic_panel_9grid.png");
    if (!panel) {
        SDL_Log("IMG_LoadTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_SetTextureScaleMode(panel, SDL_SCALEMODE_NEAREST);

    bool running = true;
    SDL_Event event;
    Uint64 start = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        int window_w = 0;
        int window_h = 0;
        SDL_GetWindowSize(window, &window_w, &window_h);
        Uint64 now = SDL_GetTicks();
        float time = (float)(now - start) / 1000.0f;
        float pulse = 0.5f + 0.5f * sinf(time * 1.25f);

        SDL_SetRenderDrawColor(renderer, 9, 7, 14, 255);
        SDL_RenderClear(renderer);

        SDL_FRect backdrop = { 0.0f, 0.0f, (float)window_w, (float)window_h };
        SDL_SetRenderDrawColor(renderer, 18, 12, 28, 255);
        SDL_RenderFillRect(renderer, &backdrop);

        float margin = 46.0f;
        float main_w = (float)window_w - margin * 2.0f;
        float main_h = (float)window_h - margin * 2.0f;
        float animated_w = main_w - 90.0f + pulse * 110.0f;
        float animated_h = main_h - 70.0f + (1.0f - pulse) * 90.0f;
        float main_x = ((float)window_w - animated_w) * 0.5f;
        float main_y = ((float)window_h - animated_h) * 0.5f;

        render_panel_tiled(renderer, panel, main_x, main_y, animated_w, animated_h, 1.0f, 1.0f);

        SDL_FRect inner = {
            main_x + 58.0f,
            main_y + 58.0f,
            animated_w - 116.0f,
            animated_h - 116.0f
        };
        //SDL_SetRenderDrawColor(renderer, 18, 10, 24, 210);
        //SDL_RenderFillRect(renderer, &inner);

        float small_w = 220.0f + pulse * 70.0f;
        float small_h = 105.0f;
        render_panel_tiled(renderer, panel, main_x + 82.0f, main_y + 82.0f,
                     small_w, small_h, 1.0f, 1.0f);
        render_panel(renderer, panel, main_x + animated_w - small_w - 82.0f,
                     main_y + animated_h - small_h - 82.0f,
                     small_w, small_h, 1.0f, 1.0f);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(panel);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
