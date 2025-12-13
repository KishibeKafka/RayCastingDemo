#include "wall2d.h"

bool isInValidArea(Vector2 pos) {
    return (pos.x >= MAP_OP_X && pos.x <= MAP_OP_X+MAP_WIDTH) && 
           (pos.y >= MAP_OP_Y && pos.y <= MAP_OP_Y+MAP_HEIGHT);
}

void CreateWall(SDL_Event& event, std::vector<Wall2D>& walls, SDL_Renderer* renderer, SDL_Color color){
    static bool isMouseDown = false;
    static Vector2 wallStartPos = {MAP_OP_X, MAP_OP_Y};

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        Vector2 start = {
            static_cast<float>(event.button.x),  // SDL鼠标坐标是int，转float
            static_cast<float>(event.button.y)
        };
        if (isInValidArea(start)) {
            isMouseDown = true;
            wallStartPos = start;  // 保存起点
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && isMouseDown) {
        Vector2 end = {
            static_cast<float>(event.button.x),
            static_cast<float>(event.button.y)
        };
        // 校验终点有效，且起点≠终点（避免画零长度的墙）
        if (isInValidArea(end) && (wallStartPos.x != end.x || wallStartPos.y != end.y)) {
            // 创建墙体并加入列表
            walls.emplace_back(wallStartPos, end, renderer, color);
        }
        // 重置状态
        isMouseDown = false;
    }
}


void Wall2D::show(){
    drawFillRect(renderer, p1.x, p1.y, p2.x - p1.x, p2.y - p1.y, color);
}