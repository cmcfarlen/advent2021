cmake_minimum_required(VERSION 3.21)
project(aoc2021)

set(CMAKE_CXX_COMPILER "clang++-14")
set(CMAKE_CXX_STANDARD 20)

set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -stdlib=libc++")

find_package(Catch2 REQUIRED)

add_executable(aoc2021 main.cpp day1.cpp day1.cpp util.h util.cpp day2.cpp day3.cpp day4.cpp day5.cpp day6.cpp day7.cpp day8.cpp day9.cpp day10.cpp)
target_link_libraries(aoc2021 Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(aoc2021)