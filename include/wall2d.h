#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include "vector2.h"
#include "draw.h"


class Wall2D{
    public:
    Vector2 p1;
    Vector2 p2;
    
    SDL_Renderer* renderer;
    SDL_Color color;
    
    Wall2D(Vector2 p1_, Vector2 p2_, SDL_Renderer* rend, SDL_Color c) : p1(p1_), p2(p2_), renderer(rend), color(c) {}
    
    void show();
};

void CreateWall(SDL_Event& event, std::vector<Wall2D>& walls, SDL_Renderer* renderer, SDL_Color color, SDL_Surface* scene_surf);