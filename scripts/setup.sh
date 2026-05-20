#!/usr/bin/env bash
# setup.sh — Clone SDL3 (fork), SDL3_image, SDL3_mixer, and ImGui (docking) next to this repo.
# Run once before building.
set -e

REPO_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PARENT="$(dirname "$REPO_DIR")"

clone_or_skip() {
    local url="$1" dest="$2" extra="${3:-}"
    if [ -d "$dest/.git" ]; then
        echo "==> Already exists, skipping: $dest"
    else
        echo "==> Cloning $url"
        git clone --depth=1 $extra "$url" "$dest"
    fi
}

clone_or_skip "https://github.com/tinieblas99/SDL.git"              "$PARENT/SDL3"
clone_or_skip "https://github.com/libsdl-org/SDL_image.git"         "$PARENT/SDL3_image"
clone_or_skip "https://github.com/libsdl-org/SDL_mixer.git"         "$PARENT/SDL3_mixer"
clone_or_skip "https://github.com/ocornut/imgui.git"                 "$PARENT/imgui" "-b docking"

echo ""
echo "==> All sources ready under $PARENT"
echo "==> Next: run scripts/install-deps-linux.sh, then scripts/build-linux.sh"
