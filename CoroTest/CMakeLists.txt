cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE FILES *.cpp)

add_executable(CoroTest ${FILES})
target_link_libraries(CoroTest pthread Coro)
