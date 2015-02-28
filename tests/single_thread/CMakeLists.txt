cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system unit_test_framework context)

file(GLOB SRC "*.cpp")

include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-std=c++1y -DBOOST_TEST_DYN_LINK -DBOOST_TEST_NO_MAIN)

add_executable(test_single_thread ${SRC})
target_link_libraries(test_single_thread coro ssl crypto)
target_link_libraries(test_single_thread ${Boost_LIBRARIES} pthread z dl)
