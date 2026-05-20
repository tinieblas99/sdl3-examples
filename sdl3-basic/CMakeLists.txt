cmake_minimum_required(VERSION 3.16)
project(sdl3_example)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)

add_executable(sdl3_example main.c)
target_link_libraries(sdl3_example SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer m)
