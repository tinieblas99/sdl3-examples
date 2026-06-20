#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL3/SDL.h>

enum {
    WINDOW_WIDTH = 768,
    WINDOW_HEIGHT = 512,
    IMAGE_WIDTH = 192,
    IMAGE_HEIGHT = 128,
    PALETTE_SIZE = 256,
    WATER_FIRST = 32,
    WATER_COLORS = 48
};

static SDL_Color base_palette[PALETTE_SIZE];

static void build_palette(void)
{
    for (int i = 0; i < PALETTE_SIZE; ++i) {
        base_palette[i] = (SDL_Color) { 0, 0, 0, 255 };
    }

    for (int i = 0; i < WATER_COLORS; ++i) {
        float t = (float)i / (float)(WATER_COLORS - 1);
        float foam = powf(sinf(t * (float)M_PI), 10.0f);
        float pulse = 0.5f + 0.5f * sinf(t * (float)M_PI * 6.0f);

        Uint8 r = (Uint8)(8.0f + 78.0f * t + 112.0f * foam);
        Uint8 g = (Uint8)(42.0f + 132.0f * t + 70.0f * foam);
        Uint8 b = (Uint8)(105.0f + 130.0f * t + 20.0f * pulse);

        base_palette[WATER_FIRST + i] = (SDL_Color) { r, g, b, 255 };
    }

    base_palette[0] = (SDL_Color) { 6, 10, 22, 255 };
}

static bool load_index_image(SDL_Texture *texture, const char *path)
{
    Uint8 pixels[IMAGE_WIDTH * IMAGE_HEIGHT];
    FILE *file = fopen(path, "rb");
    if (!file) {
        SDL_Log("Failed to open %s", path);
        return false;
    }

    size_t bytes_read = fread(pixels, 1, sizeof(pixels), file);
    fclose(file);

    if (bytes_read != sizeof(pixels)) {
        SDL_Log("Expected %zu bytes from %s, got %zu", sizeof(pixels), path, bytes_read);
        return false;
    }

    return SDL_UpdateTexture(texture, NULL, pixels, IMAGE_WIDTH);
}

static void roll_water_palette(SDL_Palette *palette, int offset)
{
    SDL_Color rolled[WATER_COLORS];

    for (int i = 0; i < WATER_COLORS; ++i) {
        int source = (i + offset) % WATER_COLORS;
        rolled[i] = base_palette[WATER_FIRST + source];
    }

    SDL_SetPaletteColors(palette, rolled, WATER_FIRST, WATER_COLORS);
}

int main(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL3 8-bit Palette Rolling", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!window || !renderer) {
        SDL_Log("Window/renderer creation failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *water = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_INDEX8,
                                          SDL_TEXTUREACCESS_STATIC,
                                          IMAGE_WIDTH, IMAGE_HEIGHT);
    SDL_Palette *palette = SDL_CreatePalette(PALETTE_SIZE);
    if (!water || !palette) {
        SDL_Log("Texture/palette creation failed: %s", SDL_GetError());
        SDL_DestroyPalette(palette);
        SDL_DestroyTexture(water);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    build_palette();
    SDL_SetPaletteColors(palette, base_palette, 0, PALETTE_SIZE);
    SDL_SetTexturePalette(water, palette);
    SDL_SetTextureScaleMode(water, SDL_SCALEMODE_NEAREST);

    if (!load_index_image(water, "assets/water_indices.bin")) {
        SDL_DestroyPalette(palette);
        SDL_DestroyTexture(water);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const SDL_FRect dst = { 0.0f, 0.0f, WINDOW_WIDTH, WINDOW_HEIGHT };
    Uint64 start = SDL_GetTicks();
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        Uint64 now = SDL_GetTicks();
        int offset = (int)((now - start) / 55) % WATER_COLORS;
        roll_water_palette(palette, offset);

        SDL_SetRenderDrawColor(renderer, 6, 10, 22, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, water, NULL, &dst);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyPalette(palette);
    SDL_DestroyTexture(water);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
