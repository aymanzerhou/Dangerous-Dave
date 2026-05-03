cmake_minimum_required(VERSION 3.16)
project(DangerousDave C)

set(CMAKE_C_STANDARD 99)

find_package(SDL2 REQUIRED)

add_executable(dave main.c)

target_link_libraries(dave SDL2::SDL2)

if (WIN32)
    target_link_libraries(dave SDL2::SDL2main)
endif()


