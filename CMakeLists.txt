cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

add_definitions(-std=c++1y -Wall)

if(BUILD_DEBUG)
	add_definitions(-O0 -g)
else()
	add_definitions(-DNDEBUG)
endif()

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system)

include_directories(${Boost_INCLUDE_DIRS} ./include)
file(GLOB SRC "src/*.cpp")

add_library(coro STATIC ${SRC})
set_target_properties(coro PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

subdirs(tests)
subdirs(examples/ws_echo_server)
