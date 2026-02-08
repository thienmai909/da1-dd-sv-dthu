cmake_minimum_required(VERSION 3.10)
project(da1-dd-sv-dthu VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(diemdanh ${SOURCES})