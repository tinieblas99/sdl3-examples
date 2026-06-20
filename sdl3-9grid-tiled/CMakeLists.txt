cmake_minimum_required(VERSION 3.16)
project(sdl3_9grid_tiled_example)

# Default to the SDL3-linux install produced by scripts/build-linux.sh
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../SDL3-linux")

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(sdl3_9grid_tiled_example main.c)
target_link_libraries(sdl3_9grid_tiled_example SDL3::SDL3 SDL3_image::SDL3_image m)
