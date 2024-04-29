cmake_minimum_required(VERSION 3.27.7) # May work on earlier versions, I'm using this one though

project(chip8)

add_executable(${PROJECT_NAME} main.c chip8.c control.c)

target_link_libraries(chip8 SDL2 SDL2_image)
