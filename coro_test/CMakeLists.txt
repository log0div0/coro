cmake_minimum_required(VERSION 2.8)

if (WIN32)
	add_definitions(/W4)
endif()

file(GLOB_RECURSE FILES *.cpp)

add_executable(coro_test ${FILES})
target_link_libraries(coro_test coro)
if (UNIX)
	target_link_libraries(coro_test pthread)
endif()
