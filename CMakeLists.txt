cmake_minimum_required(VERSION 2.8)

add_definitions(-DASIO_STANDALONE)

if (UNIX)
	add_definitions(-Wall -Wextra -std=c++14)
endif()

include_directories(
	${CMAKE_SOURCE_DIR}/3rd_party
	${CMAKE_SOURCE_DIR}
)

if (WIN32)
	if (NOT BOOST_ROOT)
		message(FATAL_ERROR "Define BOOST_ROOT variable")
	endif()
	include_directories(
		${BOOST_ROOT}
	)
endif()

add_subdirectory(coro)
add_subdirectory(coro_test)
add_subdirectory(coro_extra)
add_subdirectory(coro_extra_test)
