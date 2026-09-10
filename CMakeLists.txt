cmake_minimum_required(VERSION 3.15)
 
project(cds
    VERSION 0.1.0
    DESCRIPTION "A small library of data structures in C"
    LANGUAGES C
)
 
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
 
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()