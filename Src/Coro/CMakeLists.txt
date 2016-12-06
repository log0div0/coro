cmake_minimum_required(VERSION 2.8)

add_definitions(-DASIO_STANDALONE -DASIO_DISABLE_EPOLL -Wno-pmf-conversions)

include_directories(
	${CORO_ROOT}/Src
	${CORO_ROOT}/3rd_party/asio/asio/include
)

file(GLOB_RECURSE FILES *.cpp)

list(REMOVE_ITEM FILES ${CORO_ROOT}/Src/Coro/FiberWindows.cpp)

add_library(Coro STATIC ${FILES})
