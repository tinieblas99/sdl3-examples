# SDL3 Examples

Requires SDL3, SDL3_image, and SDL3_mixer built from source (see build scripts in the SDL3 repo).
ImGui examples also require the [docking branch of Dear ImGui](https://github.com/ocornut/imgui/tree/docking) cloned as `../imgui`.

## Examples

### sdl3-basic
Minimal SDL3 example using SDL3_image and SDL3_mixer.
- Loads and renders textures with `SDL_RenderTextureRotated` (tint color) and `SDL_RenderTextureRotatedEx` (per-vertex color — custom function added to the SDL3 fork)
- Plays looping OGG music via the new SDL3_mixer API

### sdl3-imgui
SDL3 + Dear ImGui (docking branch) integration.
- `main.cpp` — SDL scene rendered into a texture, displayed as a dockable ImGui panel
- `main2.cpp` — SDL scene rendered directly to the window with ImGui overlaid on top

## Building

```bash
cd sdl3-basic   # or sdl3-imgui
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
