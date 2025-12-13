#pragma once

#include <SDL2/SDL.h>

// 绘制填充圆
void drawFillCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius, SDL_Color color);

// 绘制线（两点之间）
void drawLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, SDL_Color color);

// 绘制填充矩形
void drawFillRect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color);