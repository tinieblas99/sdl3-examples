cmake_minimum_required(VERSION 3.16)
project(sdl3_paletted_example)

# Default to the SDL3-linux install produced by scripts/build-linux.sh
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../SDL3-linux")

find_package(SDL3 REQUIRED)

add_executable(sdl3_paletted_example main.c)
target_link_libraries(sdl3_paletted_example SDL3::SDL3 m)
