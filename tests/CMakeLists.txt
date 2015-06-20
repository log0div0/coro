cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

find_package(Boost COMPONENTS REQUIRED system thread unit_test_framework context coroutine)

file(GLOB SRC "*.cpp")

add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_NO_MAIN)

add_executable(test_coro ${SRC})
target_link_libraries(test_coro coro ssl crypto)
target_link_libraries(test_coro ${Boost_LIBRARIES} pthread z dl)
