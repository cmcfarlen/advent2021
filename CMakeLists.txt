cmake_minimum_required(VERSION 3.21)
project(aoc2021)

set(CMAKE_CXX_COMPILER "clang++-14")
set(CMAKE_CXX_STANDARD 20)

set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -stdlib=libc++")

add_executable(aoc2021 main.cpp)
