cmake_minimum_required(VERSION 2.6)

project(snake)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("${PROJECT_SOURCE_DIR}/lib")
add_subdirectory(lib)

add_executable(bin/snake snake.cpp)

target_link_libraries(bin/snake liballegro.so allegro_dialog allegro allegro_ttf allegro_font allegro_primitives allegro_image neural_network)
