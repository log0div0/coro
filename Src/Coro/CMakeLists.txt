cmake_minimum_required(VERSION 2.8)

add_definitions(-DASIO_STANDALONE -Wno-pmf-conversions)

include_directories(
	${CMAKE_SOURCE_DIR}/Src
	${CMAKE_SOURCE_DIR}/3rd_party/asio/asio/include
)

file(GLOB_RECURSE FILES *.cpp)

list(REMOVE_ITEM FILES ${CMAKE_SOURCE_DIR}/Src/Coro/FiberWindows.cpp)

add_library(Coro STATIC ${FILES})
