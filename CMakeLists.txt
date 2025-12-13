cmake_minimum_required(VERSION 3.10)

project(RayCastingDemo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/player2d.cpp
    ${PROJECT_SOURCE_DIR}/src/wall2d.cpp
    ${PROJECT_SOURCE_DIR}/src/vector2.cpp
    ${PROJECT_SOURCE_DIR}/src/draw.cpp
)

add_executable(demo ${SOURCES})

target_link_libraries(demo PRIVATE SDL2::SDL2)