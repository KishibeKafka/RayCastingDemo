find_package(SDL2 REQUIRED)

add_executable(demo3d main.cpp)

target_link_libraries(demo3d PRIVATE SDL2::SDL2)