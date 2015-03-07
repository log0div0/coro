cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0017 OLD)

include(FindBoost)
find_package(Boost COMPONENTS REQUIRED system)

add_definitions(-std=c++1y -fPIC -Wall)

include_directories(${Boost_INCLUDE_DIRS} ./include)
file(GLOB SRC "src/*.cpp")

add_library(coro STATIC ${SRC})
set_target_properties(coro PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

subdirs(tests/just_coro)
subdirs(tests/single_thread)
subdirs(examples/stress)
subdirs(examples/ws_echo_server)
