cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system)

include_directories(./)

add_definitions(-std=c++1y -fPIC)

set(SRCS
	Buffer.cpp
	Coro.cpp
	CoroMutex.cpp
	CoroPool.cpp
	CoroUtils.cpp
	NetworkIterator.cpp
	TcpServer.cpp
	TcpSocket.cpp
	ThreadPool.cpp
)

add_definitions("-Wall")

include_directories(../ ${Boost_INCLUDE_DIRS})

add_library(coro STATIC ${SRCS})
set_target_properties(coro PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

subdirs(check)
subdirs(test_coro)