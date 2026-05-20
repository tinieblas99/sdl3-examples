#!/usr/bin/env bash
# build-windows.sh — Cross-compile SDL3, SDL3_image, SDL3_mixer for Windows x86_64 (mingw-w64).
# Usage: ./scripts/build-windows.sh [INSTALL_PREFIX]
set -e

PARENT="$(cd "$(dirname "$0")/../.." && pwd)"
PREFIX="${1:-$PARENT/SDL3-win64}"
JOBS="$(nproc)"
TOOLCHAIN="$PARENT/SDL3/build-scripts/cmake-toolchain-mingw64-x86_64.cmake"

SDL3_SRC="$PARENT/SDL3"
IMAGE_SRC="$PARENT/SDL3_image"
MIXER_SRC="$PARENT/SDL3_mixer"

if ! command -v x86_64-w64-mingw32-gcc &>/dev/null; then
    echo "ERROR: mingw-w64 not found. Install with: sudo apt install mingw-w64"
    exit 1
fi

cmake_build() {
    local src="$1"; shift
    local build_dir="$1"; shift
    cmake -S "$src" -B "$build_dir" \
        -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$PREFIX" \
        -DCMAKE_PREFIX_PATH="$PREFIX" \
        "$@"
    cmake --build "$build_dir" -j"$JOBS"
    cmake --install "$build_dir"
}

echo "==> Installing to: $PREFIX"

echo "==> SDL3 (shared)"
cmake_build "$SDL3_SRC" "$SDL3_SRC/build-win64-shared" \
    -DSDL_SHARED=ON -DSDL_STATIC=OFF

echo "==> SDL3 (static)"
cmake_build "$SDL3_SRC" "$SDL3_SRC/build-win64-static" \
    -DSDL_SHARED=OFF -DSDL_STATIC=ON

echo "==> SDL3_image (shared)"
cmake_build "$IMAGE_SRC" "$IMAGE_SRC/build-win64-shared" \
    -DBUILD_SHARED_LIBS=ON

echo "==> SDL3_image (static)"
cmake_build "$IMAGE_SRC" "$IMAGE_SRC/build-win64-static" \
    -DBUILD_SHARED_LIBS=OFF

MIXER_FLAGS=(
    -DSDLMIXER_FLAC_LIBFLAC=OFF
    -DSDLMIXER_OPUS=OFF
    -DSDLMIXER_VORBIS=STB
    -DSDLMIXER_VORBIS_VORBISFILE=OFF
    -DSDLMIXER_MP3_MPG123=OFF
    -DSDLMIXER_GME=OFF
    -DSDLMIXER_SAMPLES=OFF
    -DSDLMIXER_EXAMPLES=OFF
)

echo "==> SDL3_mixer (shared)"
cmake_build "$MIXER_SRC" "$MIXER_SRC/build-win64-shared" \
    -DBUILD_SHARED_LIBS=ON "${MIXER_FLAGS[@]}"

echo "==> SDL3_mixer (static)"
cmake_build "$MIXER_SRC" "$MIXER_SRC/build-win64-static" \
    -DBUILD_SHARED_LIBS=OFF "${MIXER_FLAGS[@]}"

echo "==> Done. Libraries installed to: $PREFIX"
find "$PREFIX" \( -name "*.dll" -o -name "*.a" \) | grep -v "installed-tests\|libexec" | sort
