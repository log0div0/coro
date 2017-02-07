cmake_minimum_required(VERSION 2.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

add_definitions(-DASIO_STANDALONE -Wall -Wextra -std=c++14)

include_directories(
	${CMAKE_SOURCE_DIR}/3rd_party
	${CMAKE_SOURCE_DIR}
)

add_subdirectory(Coro)
add_subdirectory(CoroTest)
