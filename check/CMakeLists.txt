cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system unit_test_framework context)

set(SRC main.cpp
	TestBuffer.cpp
	TestCoroMutex.cpp
	TestCoroQueue.cpp
	TestCoroUtils.cpp
	TestNetworkIterator.cpp
	TestTcp.cpp
)

include_directories(../ ${Boost_INCLUDE_DIRS})
add_definitions(-std=c++1y -DBOOST_TEST_DYN_LINK -DBOOST_TEST_NO_MAIN)

add_executable(coro-test ${SRC})
target_link_libraries(coro-test coro)
target_link_libraries(coro-test ${Boost_LIBRARIES} pthread rt z dl)
