cmake_minimum_required(VERSION 3.17)
project(zadanie5 C)

set(CMAKE_C_STANDARD 11)

add_executable(zadanie5
        data.c
        data.h
        main.c
        rng.c
        rng.h)