cmake_minimum_required(VERSION 3.15)
project(git_stories C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} lib)

add_subdirectory(lib)

add_executable(git_stories src/main.c src/objects.c src/window_manager.c)
IF (WIN32)
    target_link_libraries(git_stories mingw32 SDL2main SDL2 SDL2_gfx)
else ()
    target_link_libraries(git_stories SDL2main SDL2 SDL2_gfx)
ENDIF (WIN32)
