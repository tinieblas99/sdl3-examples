#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <math.h>

/* ---- helpers from the SDL3 test example ---- */

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

/* -------------------------------------------- */

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD);

    float dpi_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_Window* window = SDL_CreateWindow("SDL3 + ImGui",
        (int)(1280 * dpi_scale), (int)(720 * dpi_scale),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderVSync(renderer, 1);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    /* ---- SDL3_image ---- */
    SDL_Texture* logo  = IMG_LoadTexture(renderer, "../test/assets/logo.png");
    SDL_Texture* logo2 = IMG_LoadTexture(renderer, "../test/assets/logo2.png");

    /* ---- SDL3_mixer ---- */
    MIX_Init();
    SDL_AudioDeviceID audio_dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    MIX_Mixer* mixer = MIX_CreateMixerDevice(audio_dev, nullptr);
    MIX_Audio* music = MIX_LoadAudio(mixer, "../test/assets/music.ogg", false);
    MIX_Track* track = MIX_CreateTrack(mixer);
    MIX_SetTrackAudio(track, music);
    MIX_SetTrackLoops(track, -1);
    MIX_PlayTrack(track, 0);

    /* ---- scene render texture (640x480) ---- */
    const int SCENE_W = 640, SCENE_H = 480;
    SDL_Texture* scene_tex = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCENE_W, SCENE_H);

    /* ---- ImGui ---- */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(dpi_scale);
    ImGui::GetStyle().FontScaleDpi = dpi_scale;
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    /* ---- imgui-controlled params ---- */
    ImVec4 bg_color    = { 0.08f, 0.08f, 0.20f, 1.0f };
    float  tint[3]     = { 1.0f, 0.5f, 0.1f };
    float  speed       = 60.0f;
    bool   music_on    = true;
    bool   show_demo   = false;

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

        /* ---- render SDL scene into scene_tex ---- */
        SDL_SetRenderTarget(renderer, scene_tex);
        SDL_SetRenderDrawColorFloat(renderer, bg_color.x, bg_color.y, bg_color.z, 1.0f);
        SDL_RenderClear(renderer);

        if (logo) {
            float lw, lh;
            SDL_GetTextureSize(logo, &lw, &lh);
            SDL_FRect logo_dst = { (SCENE_W - lw) / 2, 20, lw, lh };
            SDL_RenderTexture(renderer, logo, nullptr, &logo_dst);
            render_texture_ex(renderer, logo, 160, 300, pulse * 0.6f, angle,
                              tint[0], tint[1], tint[2]);
        }
        if (logo2) {
            SDL_FColor colors[4] = {
                { 1.0f, 0.2f, 0.2f, 1.0f },
                { 0.2f, 1.0f, 0.2f, 1.0f },
                { 0.2f, 0.2f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 0.2f, 1.0f },
            };
            render_texture_vertex_color(renderer, logo2, 480, 300,
                                        pulse * 0.6f, -angle * 0.7, colors);
        }
        SDL_SetRenderTarget(renderer, nullptr);

        /* ---- ImGui frame ---- */
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        /* Scene viewport window */
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Scene");
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)(intptr_t)scene_tex, avail,
                     ImVec2(0, 0), ImVec2(1, 1));
        ImGui::End();
        ImGui::PopStyleVar();

        /* Controls window */
        ImGui::Begin("Controls");
        ImGui::SeparatorText("Scene");
        ImGui::ColorEdit3("Background", (float*)&bg_color);
        ImGui::ColorEdit3("Tint (left logo)", tint);
        ImGui::SliderFloat("Rotation speed", &speed, 0.0f, 360.0f, "%.0f deg/s");
        ImGui::SeparatorText("Audio");
        if (ImGui::Checkbox("Music", &music_on))
            music_on ? MIX_ResumeTrack(track) : MIX_PauseTrack(track);
        ImGui::SeparatorText("Info");
        ImGui::Text("%.1f FPS", io.Framerate);
        ImGui::Checkbox("ImGui demo", &show_demo);
        ImGui::End();

        if (show_demo) ImGui::ShowDemoWindow(&show_demo);

        /* ---- composite ---- */
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyTexture(scene_tex);
    MIX_StopTrack(track, 0);
    MIX_DestroyTrack(track);
    MIX_DestroyAudio(music);
    MIX_DestroyMixer(mixer);
    SDL_CloseAudioDevice(audio_dev);
    MIX_Quit();
    SDL_DestroyTexture(logo);
    SDL_DestroyTexture(logo2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
