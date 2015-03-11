cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system unit_test_framework context)

file(GLOB SRC "*.cpp")

include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-std=c++1y -DBOOST_TEST_DYN_LINK -DBOOST_TEST_NO_MAIN)

add_executable(test_multi_thread ${SRC})
target_link_libraries(test_multi_thread coro ssl crypto)
target_link_libraries(test_multi_thread ${Boost_LIBRARIES} pthread z dl)
