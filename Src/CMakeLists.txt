cmake_minimum_required(VERSION 2.8)

project(coro)

add_definitions(-Wall -Wextra -g -O3 -std=c++14)

add_subdirectory(Coro)
add_subdirectory(CoroTest)
