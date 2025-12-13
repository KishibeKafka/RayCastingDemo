#include "player2d.h"

#define DEG2RAD(deg) ((deg) * M_PI / 180.0f)

void Player2D::show(){
    drawFillCircle(renderer, pos.x, pos.y, radius, color);
    float len = 100.0f;
    Vector2 left_bound = Vector2Add(pos, Vector2MultiplyScalar(Vector2Subtract(dir, plane), len));
    Vector2 right_bound = Vector2Add(pos, Vector2MultiplyScalar(Vector2Add(dir, plane), len));
    drawLine(renderer, pos.x, pos.y, left_bound.x, left_bound.y, color);
    drawLine(renderer, pos.x, pos.y, right_bound.x, right_bound.y, color);
}

void Player2D::handleInput(SDL_Event& event){
    if (event.type == SDL_KEYDOWN){
        switch( event.key.keysym.sym )
        {
            case SDLK_w: move(5); break;
            case SDLK_s: move(-5); break;
            case SDLK_a: lookaround(-5); break;
            case SDLK_d: lookaround(5); break;    
        }
    }
}

void Player2D::move(int speed){
    Vector2 dir_normalized = Vector2Normalize(dir);
    Vector2 delta = Vector2MultiplyScalar(dir_normalized, (float)speed);
    pos = Vector2Add(pos, delta);
    if (pos.x < MAP_OP_X)
        pos.x = MAP_OP_X;
    else if (pos.x > MAP_OP_X + MAP_WIDTH)
    pos.x = MAP_OP_X + MAP_WIDTH;
    if (pos.y < MAP_OP_Y)
        pos.y = MAP_OP_Y;
    else if (pos.y > MAP_OP_Y + MAP_HEIGHT)
        pos.y = MAP_OP_Y + MAP_HEIGHT;
}

void Player2D::lookaround(int look_speed){
    float angle_rad = DEG2RAD(look_speed);
    dir = Vector2Rotate(dir, angle_rad);
    // 平面与方向垂直，需同步旋转
    plane = Vector2Rotate(plane, angle_rad);
}