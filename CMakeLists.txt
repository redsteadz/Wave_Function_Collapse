cmake_minimum_required(VERSION 3.1)

project(WFC)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_BUILD_TYPE Debug)
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

find_package(Raylib REQUIRED)

add_executable(WFC main.cpp)

target_link_libraries(WFC PRIVATE -lraylib)
