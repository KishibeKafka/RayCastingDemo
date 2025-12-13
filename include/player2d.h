#pragma once

#include <SDL2/SDL.h>
#include "vector2.h"
#include "draw.h"

class Player2D{
public:
    Vector2 pos;
    Vector2 dir;
    Vector2 plane;

    SDL_Renderer* renderer;
    SDL_Color color;
    int radius;

    Player2D(Vector2 p, SDL_Renderer* rend, SDL_Color c) : pos(p), dir({1,0}), plane({0,1}), renderer(rend), color(c), radius(10) {}

    void show();
    void handleInput(SDL_Event& event);

    void move(int speed);
    void lookaround(int look_speed);
};
