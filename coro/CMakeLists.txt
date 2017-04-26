cmake_minimum_required(VERSION 2.8)

add_definitions(-std=gnu++14)

if (WIN32)
	add_definitions(/W4)
endif()

file(GLOB_RECURSE FILES *.cpp)
if (UNIX)
	list(REMOVE_ITEM FILES ${CMAKE_CURRENT_SOURCE_DIR}/FiberWindows.cpp)
endif()
if (WIN32)
	list(REMOVE_ITEM FILES ${CMAKE_CURRENT_SOURCE_DIR}/FiberLinux.cpp)
endif()

add_library(coro STATIC ${FILES})
