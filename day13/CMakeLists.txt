cmake_minimum_required(VERSION 3.9.1)
project(day13)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(target day13)
 
set(files main.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${target} ${files})