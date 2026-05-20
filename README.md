# SDL3 Examples

SDL3 examples using SDL3_image, SDL3_mixer, and Dear ImGui (docking branch).
Includes a custom `SDL_RenderTextureRotatedEx` function added to the [SDL3 fork](https://github.com/tinieblas99/SDL).

## Examples

| Directory | Description |
|-----------|-------------|
| `sdl3-basic` | SDL3 + SDL3_image + SDL3_mixer. Rotating/tinted textures, per-vertex color via `SDL_RenderTextureRotatedEx`, looping OGG music. |
| `sdl3-imgui` | `main.cpp` — SDL scene in a dockable ImGui panel. `main2.cpp` — ImGui overlaid directly on the SDL scene. |

## Setup

### 1. Install system dependencies (Ubuntu/Debian)
```bash
./scripts/install-deps-linux.sh
# For Windows cross-compilation:
sudo apt install mingw-w64
```

### 2. Clone SDL3 and dependencies
```bash
./scripts/setup.sh
```
This clones the following repos **next to** this repository:
- `../SDL3` — SDL3 fork (with `SDL_RenderTextureRotatedEx`)
- `../SDL3_image`
- `../SDL3_mixer`
- `../imgui` — docking branch (required for sdl3-imgui)

### 3. Build the libraries

**Linux:**
```bash
./scripts/build-linux.sh
# Installs to ../SDL3-linux/  (override: ./scripts/build-linux.sh /your/prefix)
```

**Windows (cross-compile from Linux):**
```bash
./scripts/build-windows.sh
# Installs to ../SDL3-win64/  (override: ./scripts/build-windows.sh /your/prefix)
```

### 4. Build an example
```bash
cd sdl3-basic        # or sdl3-imgui
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/sdl3_example
```

## Directory layout (after setup)

```
parent/
├── sdl3-examples/      ← this repo
│   ├── scripts/
│   ├── sdl3-basic/
│   └── sdl3-imgui/
├── SDL3/               ← cloned by setup.sh
├── SDL3_image/
├── SDL3_mixer/
├── imgui/
├── SDL3-linux/         ← created by build-linux.sh
└── SDL3-win64/         ← created by build-windows.sh
```
