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

SDL_Color Player2D::getPixelColor(int x, int y){
    SDL_Color color = {0, 0, 0, 255};
    if (!scene_surface) return color;
    
    // 校验坐标有效性
    if (x < 0 || x >= scene_surface->w || y < 0 || y >= scene_surface->h) {
        return color;
    }

    // 计算像素在内存中的偏移（SDL_Surface的像素格式需为32位RGBA/ARGB）
    uint32_t* pixels = (uint32_t*)scene_surface->pixels;
    uint32_t pixel = pixels[y * scene_surface->w + x];

    // 解析像素的RGBA分量（适配不同的像素格式）
    SDL_GetRGBA(pixel, scene_surface->format, &color.r, &color.g, &color.b, &color.a);
    return color;
}

void Player2D::ray_casting(){
    for (int i = 0; i < RAY_NUM; ++i){
        float ray_x = 2 * i / (float)RAY_NUM - 1; // 映射到[-1,1)
        Vector2 rayDir = Vector2Add(dir, Vector2MultiplyScalar(plane, ray_x));
        int mapX = (int)pos.x, mapY = (int)pos.y;
        float sideDistX, sideDistY; // 距离下一格边界要走的距离
        float deltaDistX = (rayDir.x == 0) ? 1e20 : std::abs(1/rayDir.x); // x方向上前进一格，光线经过的距离
        float deltaDistY = (rayDir.y == 0) ? 1e20 : std::abs(1/rayDir.y);
        int stepX, stepY;
        int hit = 0;
        int side = 0; // 墙的方向
        if (rayDir.x < 0){
            stepX = -1;
            sideDistX = (pos.x - mapX) * deltaDistX;
        } else{
            stepX = 1;
            sideDistX = (mapX+1-pos.x) * deltaDistX;
        }
        if (rayDir.y < 0){
            stepY = -1;
            sideDistY = (pos.y - mapY) * deltaDistY;
        } else{
            stepY = 1;
            sideDistY = (mapY+1-pos.y) * deltaDistY;
        }
        // 光线前进直到撞墙
        while (!hit){
            if (sideDistX < sideDistY){
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else{
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (mapX < MAP_OP_X || mapX > MAP_OP_X+MAP_WIDTH || mapY < MAP_OP_Y || mapY > MAP_OP_Y+MAP_HEIGHT){
                hit = 1;
                side = -1;
                continue;
            }
            SDL_Color pixel_color = getPixelColor(mapX, mapY);
            if (pixel_color.b >= 230)
                hit = 1;
        }
        float wallDist;
        if (side == 0)
            wallDist = sideDistX - deltaDistX;
        else if (side == 1)
            wallDist = sideDistY - deltaDistY;
        else
            wallDist = -1.0f; // 撞到边界了

        if (wallDist > 0){
            float wallX = i * MAP_WIDTH/RAY_NUM;
            int wallHeight = (int)(20 * MAP_HEIGHT / wallDist);
            int y_min = -wallHeight/2 + MAP_HEIGHT/2;
            SDL_Color c = {0, 0, 255, 255};
            if (side == 1) c.b = 128;
            drawFillRect(renderer, wallX, y_min, MAP_WIDTH/RAY_NUM, wallHeight, c);
        }
    }
}

