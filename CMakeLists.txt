cmake_minimum_required(VERSION 3.10)
project(MemoryStress C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(MemoryStress
        main.c
        MemoryStress.c
        MemoryStress.h)
