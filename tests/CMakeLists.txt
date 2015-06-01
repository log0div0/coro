cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system unit_test_framework context log thread coroutine)

file(GLOB SRC "*.cpp")

include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-DBOOST_TEST_DYN_LINK -DBOOST_TEST_NO_MAIN -DBOOST_LOG_DYN_LINK)

add_executable(test_coro ${SRC})
target_link_libraries(test_coro coro ssl crypto)
target_link_libraries(test_coro ${Boost_LIBRARIES} pthread z dl)
