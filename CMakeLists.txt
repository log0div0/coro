cmake_minimum_required(VERSION 2.8)

add_definitions(-DASIO_STANDALONE -Wall -Wextra -std=c++14)

include_directories(
	${CMAKE_SOURCE_DIR}/3rd_party
	${CMAKE_SOURCE_DIR}
)

add_subdirectory(Coro)
add_subdirectory(CoroTest)
