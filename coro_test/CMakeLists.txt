cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE FILES *.cpp)

add_executable(coro_test ${FILES})
target_link_libraries(coro_test pthread coro)
