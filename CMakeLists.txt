cmake_minimum_required(VERSION 3.27.7) # May work on earlier versions, I'm using this one though

project(chip8)

add_subdirectory(./src)

find_package(SDL2 REQUIRED)
target_link_libraries(chip8 SDL2::SDL2)
