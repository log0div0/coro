cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system context)

set(SRC main.cpp)

include_directories(../../)
include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-std=c++1y)

add_executable(coro-example-ws-echo-server ${SRC})
target_link_libraries(coro-example-ws-echo-server coro ssl crypto)
target_link_libraries(coro-example-ws-echo-server ${Boost_LIBRARIES} pthread rt z dl)
