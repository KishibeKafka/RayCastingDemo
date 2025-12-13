#include <iostream>
#include <SDL2/SDL.h>
#include "vector2.h"
#include "player2d.h"
#include "wall2d.h"
#include "draw.h"

using namespace std;

int main(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL 初始化失败：" << SDL_GetError() << endl;
        return 1;
    }

    // 创建窗口
    SDL_Window* window = SDL_CreateWindow(
        "RayCastingDemo",                  // 窗口标题
        SDL_WINDOWPOS_CENTERED,      // x 坐标
        SDL_WINDOWPOS_CENTERED,      // y 坐标
        SCREEN_WIDTH, SCREEN_HEIGHT,                    // 宽高
        SDL_WINDOW_SHOWN             // 窗口标志
    );

    if (!window) {
        cerr << "窗口创建失败：" << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }
    // 创建渲染器
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        cerr << "渲染器创建失败：" << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool isRunning = true;
    SDL_Event event;
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color blue = {0, 0, 255, 255};
    SDL_Color white = {255, 255, 255, 255};
    Player2D player({MAP_OP_X + MAP_WIDTH/2, MAP_OP_Y + MAP_HEIGHT/2}, renderer, red);
    vector<Wall2D> walls;
    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                default:
                    break;
            }
            player.handleInput(event);
            CreateWall(event, walls, renderer, blue);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        // 绘制2d地图和3d场景的边界线
        drawFillRect(renderer, MAP_OP_X-5, 0, 10, MAP_HEIGHT, white);

        for (auto& wall : walls)
            wall.show();

        player.show();

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 清理资源
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0; 
}