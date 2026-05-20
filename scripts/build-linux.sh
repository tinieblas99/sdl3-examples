#!/usr/bin/env bash
# build-linux.sh — Build SDL3, SDL3_image, SDL3_mixer for Linux.
# Usage: ./scripts/build-linux.sh [INSTALL_PREFIX]
set -e

PARENT="$(cd "$(dirname "$0")/../.." && pwd)"
PREFIX="${1:-$PARENT/SDL3-linux}"
JOBS="$(nproc)"

SDL3_SRC="$PARENT/SDL3"
IMAGE_SRC="$PARENT/SDL3_image"
MIXER_SRC="$PARENT/SDL3_mixer"

cmake_build() {
    local src="$1"; shift
    local build_dir="$1"; shift
    cmake -S "$src" -B "$build_dir" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$PREFIX" \
        -DCMAKE_PREFIX_PATH="$PREFIX" \
        "$@"
    cmake --build "$build_dir" -j"$JOBS"
    cmake --install "$build_dir"
}

echo "==> Installing to: $PREFIX"

echo "==> SDL3 (shared + static)"
cmake_build "$SDL3_SRC" "$SDL3_SRC/build-linux" \
    -DSDL_SHARED=ON -DSDL_STATIC=ON

echo "==> SDL3_image (shared + static)"
cmake_build "$IMAGE_SRC" "$IMAGE_SRC/build-linux" \
    -DSDLIMAGE_SHARED=ON -DSDLIMAGE_STATIC=ON

echo "==> SDL3_mixer (shared + static)"
cmake_build "$MIXER_SRC" "$MIXER_SRC/build-linux" \
    -DSDLMIXER_SHARED=ON -DSDLMIXER_STATIC=ON \
    -DSDLMIXER_FLAC_LIBFLAC=OFF \
    -DSDLMIXER_OPUS=OFF \
    -DSDLMIXER_VORBIS=STB \
    -DSDLMIXER_VORBIS_VORBISFILE=OFF \
    -DSDLMIXER_MP3_MPG123=OFF \
    -DSDLMIXER_GME=OFF \
    -DSDLMIXER_SAMPLES=OFF \
    -DSDLMIXER_EXAMPLES=OFF

echo "==> Done. Libraries installed to: $PREFIX"
find "$PREFIX" \( -name "*.so*" -o -name "*.a" \) | grep -v "installed-tests\|libexec" | sort
