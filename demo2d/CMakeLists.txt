find_package(SDL2 REQUIRED)

set(SOURCES
    main.cpp
    player2d.cpp
    wall2d.cpp
    vector2.cpp
    draw.cpp
)

add_executable(demo2d ${SOURCES})

target_link_libraries(demo2d PRIVATE SDL2::SDL2)