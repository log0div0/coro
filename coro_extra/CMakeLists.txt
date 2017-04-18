cmake_minimum_required(VERSION 2.8)

add_definitions(-std=gnu++14)

file(GLOB_RECURSE FILES *.cpp)

add_library(coro_extra STATIC ${FILES})
