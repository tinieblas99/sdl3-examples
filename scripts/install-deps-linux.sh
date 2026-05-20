#!/usr/bin/env bash
# install-deps-linux.sh — Install build dependencies on Ubuntu/Debian.
set -e

sudo apt-get update -qq
sudo apt-get install -y \
    cmake gcc g++ make \
    libx11-dev libxext-dev libxcursor-dev libxrandr-dev libxinerama-dev \
    libxi-dev libxss-dev libxtst-dev \
    libwayland-dev libxkbcommon-dev \
    libegl1-mesa-dev libgles2-mesa-dev libgl1-mesa-dev \
    libasound2-dev libpulse-dev libudev-dev libdbus-1-dev libibus-1.0-dev \
    libpng-dev libjpeg-dev libwebp-dev libvorbis-dev libflac-dev

echo "==> Dependencies installed."
echo "==> For Windows cross-compilation also run: sudo apt install mingw-w64"
