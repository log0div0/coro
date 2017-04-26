cmake_minimum_required(VERSION 2.8)

file(GLOB FILES "*.cpp")

add_executable(coro_extra_test ${FILES})
add_dependencies(coro_extra_test coro coro_extra)
target_link_libraries(coro_extra_test coro_extra coro ${Boost_LIBRARIES})
if (UNIX)
	target_link_libraries(coro_extra_test pthread ssl crypto)
endif()