cmake_minimum_required(VERSION 3.15)
project(gs_rendering C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_ASAN "ENABLE_ASAN" OFF)

if(ENABLE_ASAN)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
endif()

set(GO_INPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/git_info)
set(GO_OUTPUT_BINARY ${CMAKE_CURRENT_BINARY_DIR}/git_stories)

add_custom_target(go-build)
add_custom_command(TARGET go-build
        WORKING_DIRECTORY ${GO_INPUT_DIRECTORY}
        COMMAND go generate
        COMMAND go build -o ${GO_OUTPUT_BINARY})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} lib)

add_subdirectory(lib)

set(PROTOBUF_INPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/protobuf)
set(PROTOBUF_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

set(PROTOBUF_ARGUMENTS --proto_path=\"${PROTOBUF_INPUT_DIRECTORY}\" --c_out=\"${PROTOBUF_OUTPUT_DIRECTORY}\" \"config.proto\")

add_custom_target(gen-protobuf)
add_custom_command(TARGET gen-protobuf
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROTOBUF_OUTPUT_DIRECTORY}
        COMMAND protoc ${PROTOBUF_ARGUMENTS})

include_directories(${PROTOBUF_OUTPUT_DIRECTORY})

set(PROTOBUF_GENERATED "${PROTOBUF_OUTPUT_DIRECTORY}/config.pb-c.c")
set_source_files_properties(${PROTOBUF_GENERATED} PROPERTIES GENERATED TRUE)

add_executable(gs_rendering
        src/main.c
        src/objects.c
        src/window_manager.c
        src/status.c
        src/render.c
        src/utils.c
        src/phisics.c
        src/phisics.h
        src/vector.c)


add_library(generated STATIC ${PROTOBUF_GENERATED})
add_dependencies(generated gen-protobuf)
target_link_libraries(generated protobuf-c)

target_link_libraries(gs_rendering SDL2main SDL2 SDL2_gfx m protobuf-c generated)
set_target_properties(gs_rendering PROPERTIES COMPILE_FLAGS "-Wall -Werror")
add_dependencies(gs_rendering go-build)

find_program(iwyu_path NAMES include-what-you-use iwyu)
if(iwyu_path)
    set_property(TARGET gs_rendering PROPERTY C_INCLUDE_WHAT_YOU_USE ${iwyu_path})
else()
    message(WARNING "Could not find the program include-what-you-use")
endif()
