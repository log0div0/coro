cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system unit_test_framework context)

set(SRC main.cpp
	TestCoro.cpp
)

include_directories(../ ${Boost_INCLUDE_DIRS})
add_definitions(-std=c++11 -DBOOST_TEST_DYN_LINK -DBOOST_TEST_NO_MAIN)

add_executable(coro-test-coro ${SRC})
target_link_libraries(coro-test-coro coro)
target_link_libraries(coro-test-coro ${Boost_LIBRARIES} pthread rt z dl)
