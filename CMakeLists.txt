cmake_minimum_required(VERSION 2.8)

add_definitions(-DASIO_STANDALONE -Wall -Wextra -std=c++14)

include_directories(
	${CMAKE_SOURCE_DIR}/3rd_party
	${CMAKE_SOURCE_DIR}
)

add_subdirectory(coro)
add_subdirectory(coro_test)
add_subdirectory(coro_extra)
add_subdirectory(coro_extra_test)
