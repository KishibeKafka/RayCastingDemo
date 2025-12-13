#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "vector2.h"
#include "draw.h"

const int RAY_NUM = 256;

class Player2D{
public:
    Vector2 pos;
    Vector2 dir;
    Vector2 plane;

    SDL_Renderer* renderer;
    SDL_Color color;
    int radius;

    SDL_Surface* scene_surface;

    Player2D(Vector2 p, SDL_Renderer* rend, SDL_Color c, SDL_Surface* surf) : pos(p), dir({1,0}),
                plane({0,1}), renderer(rend), color(c), radius(10), scene_surface(surf)
    {
    }

    void show();
    void handleInput(SDL_Event& event);

    void move(int speed);
    void lookaround(int look_speed);

    void ray_casting();
    SDL_Color getPixelColor(int x, int y);
};
