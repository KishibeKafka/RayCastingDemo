cmake_minimum_required(VERSION 3.10)

project(RayCastingDemo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(demo2d)
add_subdirectory(demo3d)