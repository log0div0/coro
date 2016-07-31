cmake_minimum_required(VERSION 2.8)

add_definitions(-DASIO_STANDALONE)

include_directories(
	${CMAKE_SOURCE_DIR}/Src
	${CMAKE_SOURCE_DIR}/3rd_party/asio/asio/include
	${CMAKE_SOURCE_DIR}/3rd_party/Catch/single_include
)

file(GLOB_RECURSE FILES *.cpp)

add_executable(CoroTest ${FILES})
target_link_libraries(CoroTest pthread Coro)