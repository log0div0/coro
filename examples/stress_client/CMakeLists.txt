cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system context)

file(GLOB SRC "*.cpp")

include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-std=c++1y)

add_executable(example_stress_client ${SRC})
target_link_libraries(example_stress_client coro)
target_link_libraries(example_stress_client ${Boost_LIBRARIES} pthread z dl)
