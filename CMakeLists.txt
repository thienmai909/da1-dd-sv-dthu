cmake_minimum_required(VERSION 3.15)
project(da1-dd-sv-dthu VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES src/main.cpp
            src/stats.cpp)

add_executable(diemdanh ${SOURCES})

target_include_directories(diemdanh PRIVATE ${PROJECT_SOURCE_DIR}/include)