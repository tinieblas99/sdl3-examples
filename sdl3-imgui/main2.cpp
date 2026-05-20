#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <math.h>

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

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD);

    SDL_Window*   window   = SDL_CreateWindow("SDL3 + ImGui overlay", 640, 480, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);

    SDL_Texture* logo  = IMG_LoadTexture(renderer, "../test/assets/logo.png");
    SDL_Texture* logo2 = IMG_LoadTexture(renderer, "../test/assets/logo2.png");

    MIX_Init();
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    MIX_Mixer* mixer = MIX_CreateMixerDevice(dev, nullptr);
    MIX_Audio* music = MIX_LoadAudio(mixer, "../test/assets/music.ogg", false);
    MIX_Track* track = MIX_CreateTrack(mixer);
    MIX_SetTrackAudio(track, music);
    MIX_SetTrackLoops(track, -1);
    MIX_PlayTrack(track, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    /* make windows semi-transparent so the scene shows through */
    ImGui::GetStyle().Alpha = 0.85f;
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    float  tint[3]  = { 1.0f, 0.5f, 0.1f };
    float  speed    = 60.0f;
    bool   music_on = true;
    bool   show_demo = false;
    double angle = 0.0;
    Uint64 last  = SDL_GetTicks();

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL3_ProcessEvent(&e);
            if (e.type == SDL_EVENT_QUIT) running = false;
        }

        Uint64 now = SDL_GetTicks();
        double dt  = (now - last) / 1000.0;
        last = now;
        angle += speed * dt;
        float pulse = 1.5f + 0.5f * sinf((float)(now / 1000.0));

        /* ---- SDL scene (same as test example) ---- */
        int ww, wh;
        SDL_GetWindowSize(window, &ww, &wh);

        SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
        SDL_RenderClear(renderer);

        if (logo) {
            float lw, lh;
            SDL_GetTextureSize(logo, &lw, &lh);
            SDL_FRect dst = { (ww - lw) / 2, 20, lw, lh };
            SDL_RenderTexture(renderer, logo, nullptr, &dst);
            render_texture_ex(renderer, logo, ww * 0.31f, wh * 0.62f, pulse * 0.6f, angle,
                              tint[0], tint[1], tint[2]);
        }
        if (logo2) {
            SDL_FColor colors[4] = {
                { 1.0f, 0.2f, 0.2f, 1.0f },
                { 0.2f, 1.0f, 0.2f, 1.0f },
                { 0.2f, 0.2f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 0.2f, 1.0f },
            };
            render_texture_vertex_color(renderer, logo2, ww * 0.70f, wh * 0.62f,
                                        pulse * 0.6f, -angle * 0.7, colors);
        }

        /* ---- ImGui overlay (no SDL_RenderClear between them) ---- */
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({10, 10}, ImGuiCond_Once);
        ImGui::SetNextWindowSize({220, 0}, ImGuiCond_Once);
        ImGui::Begin("Controls");
        ImGui::ColorEdit3("Tint", tint);
        ImGui::SliderFloat("Speed", &speed, 0.0f, 360.0f, "%.0f deg/s");
        if (ImGui::Checkbox("Music", &music_on))
            music_on ? MIX_ResumeTrack(track) : MIX_PauseTrack(track);
        ImGui::Checkbox("Demo", &show_demo);
        ImGui::Text("%.1f FPS", io.Framerate);
        ImGui::End();

        if (show_demo) ImGui::ShowDemoWindow(&show_demo);

        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
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
