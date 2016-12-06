cmake_minimum_required(VERSION 2.8)

add_definitions(-DASIO_STANDALONE -DASIO_DISABLE_EPOLL)

include_directories(
	${CORO_ROOT}/Src
	${CORO_ROOT}/3rd_party/asio/asio/include
	${CORO_ROOT}/3rd_party/Catch/single_include
)

file(GLOB_RECURSE FILES *.cpp)

add_executable(CoroTest ${FILES})
target_link_libraries(CoroTest pthread Coro)