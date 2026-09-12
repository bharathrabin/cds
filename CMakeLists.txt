cmake_minimum_required(VERSION 3.15)

project(cds
    VERSION 0.1.0
    DESCRIPTION "A small library data structures in C"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined -g -O0)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(cds
    src/vector.c
    src/list.c
    src/stack.c
)

target_include_directories(cds
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_options(cds PRIVATE
    -Wall -Wextra -Wpedantic
)

enable_testing()
add_subdirectory(tests)