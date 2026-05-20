#include <stdbool.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>

/* Draw a texture with rotation, uniform scale, and a tint color.
 * cx,cy  - center position on screen
 * scale  - uniform scale factor
 * angle  - rotation in degrees
 * r,g,b  - tint color (0.0–1.0) */
static void render_texture_ex(SDL_Renderer *renderer, SDL_Texture *tex,
                               float cx, float cy, float scale, double angle,
                               float r, float g, float b)
{
    float w, h;
    SDL_GetTextureSize(tex, &w, &h);
    w *= scale; h *= scale;
    SDL_FRect dst = { cx - w / 2, cy - h / 2, w, h };
    SDL_SetTextureColorModFloat(tex, r, g, b);
    SDL_RenderTextureRotated(renderer, tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE);
    SDL_SetTextureColorModFloat(tex, 1, 1, 1);
}

/* Draw a texture with rotation, uniform scale, and per-vertex colors using
 * SDL_RenderTextureRotatedEx (added to SDL3 source).
 * colors[4]: top-left, top-right, bottom-right, bottom-left */
static void render_texture_vertex_color(SDL_Renderer *renderer, SDL_Texture *tex,
                                        float cx, float cy, float scale, double angle,
                                        SDL_FColor colors[4])
{
    float w, h;
    SDL_GetTextureSize(tex, &w, &h);
    w *= scale; h *= scale;
    SDL_FRect dst = { cx - w / 2, cy - h / 2, w, h };
    SDL_RenderTextureRotatedEx(renderer, tex, NULL, &dst, angle, NULL, SDL_FLIP_NONE, colors);
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    SDL_Window   *window   = SDL_CreateWindow("SDL3 Example", 640, 480, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    SDL_Texture *logo  = IMG_LoadTexture(renderer, "assets/logo.png");
    SDL_Texture *logo2 = IMG_LoadTexture(renderer, "assets/logo2.png");

    /* SDL3_mixer */
    MIX_Init();
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    MIX_Mixer *mixer = MIX_CreateMixerDevice(dev, NULL);
    MIX_Audio *music = MIX_LoadAudio(mixer, "assets/music.ogg", false);
    MIX_Track *track = MIX_CreateTrack(mixer);
    MIX_SetTrackAudio(track, music);
    MIX_SetTrackLoops(track, -1);
    MIX_PlayTrack(track, 0);

    /* static logo position */
    float lw = 0, lh = 0;
    if (logo) SDL_GetTextureSize(logo, &lw, &lh);
    SDL_FRect logo_dst = { (640 - lw) / 2, 20, lw, lh };

    double angle = 0;
    Uint64 last = SDL_GetTicks();

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_EVENT_QUIT) running = false;

        Uint64 now = SDL_GetTicks();
        double dt = (now - last) / 1000.0;
        last = now;
        angle += 60.0 * dt;

        float scale = 1.5f + 0.5f * sinf((float)(now / 1000.0));

        SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
        SDL_RenderClear(renderer);

        /* static SDL logo at top */
        if (logo) SDL_RenderTexture(renderer, logo, NULL, &logo_dst);

        /* rotating + scaling + orange-tinted logo */
        if (logo)
            render_texture_ex(renderer, logo, 200, 300, scale * 0.6f, angle,
                              1.0f, 0.5f, 0.1f);

        /* rotating logo with per-vertex rainbow colors via SDL_RenderTextureRotatedEx */
        if (logo2) {
            SDL_FColor colors[4] = {
                { 1.0f, 0.2f, 0.2f, 1.0f }, /* top-left:     red    */
                { 0.2f, 1.0f, 0.2f, 1.0f }, /* top-right:    green  */
                { 0.2f, 0.2f, 1.0f, 1.0f }, /* bottom-right: blue   */
                { 1.0f, 1.0f, 0.2f, 1.0f }, /* bottom-left:  yellow */
            };
            render_texture_vertex_color(renderer, logo2, 450, 300,
                                        scale * 0.6f, -angle * 0.7, colors);
        }

        SDL_RenderPresent(renderer);
    }

    MIX_StopTrack(track, 0);
    MIX_DestroyTrack(track);
    MIX_DestroyAudio(music);
    MIX_DestroyMixer(mixer);
    SDL_CloseAudioDevice(dev);
    MIX_Quit();
    SDL_DestroyTexture(logo);
    SDL_DestroyTexture(logo2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
