cmake_minimum_required(VERSION 2.8)

project(coro)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

add_definitions(-Wall -Wextra -g -O3 -std=c++14)

add_subdirectory(Src/Coro)
add_subdirectory(Src/CoroTest)
