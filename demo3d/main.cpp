#include <iostream>
#include <SDL2/SDL.h>
#include <cmath>

using namespace std;

// 三维空间大小
const int SPACE_WIDTH = 200;
const int SPACE_HEIGHT = 200;
const int SPACE_LENGTH = 200;

// 二维屏幕大小
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

#define R_AT(rgbas, i, j, k) rgbas[i*SPACE_HEIGHT*SPACE_LENGTH*4+j*SPACE_LENGTH*4+k*4]

// 类型枚举
enum VoxelType {Air, Cube_Blue, Cube_Red, Sphere_Green};

// 函数定义
void GenerateVoxels(int *Voxels);
void GenerateCube(int x, int y, int z, int d, int *Voxels, VoxelType type);
void GenerateSphere(int x, int y, int z, int r, int *Voxels, VoxelType type);
void GenerateRBGAs(int *Voxels, float *RGBAs);
void RayCasting(float *pos, float *dir, float *planeX, float *planeY, float *RGBAs, float *Pixels);
int PosValid(float x, float y, float z);
void TriInterpolation(float *rgba, float *pos, float *RGBAs);
void TransPixels(Uint8 *sdl_pixels, float *Pixels);

// 向量操作
void Vector3Copy(float *dest, float *src);
void Vector3Add(float *dest, float *src1, float *src2);
void Vector3Scale(float *dest, float *vector, float scalar);
void Vector3Normalize(float *dest, float *src);
void Vector3Rotate(float *dest, float *src, float *matrix);
void Vector3GetRotateXMatrix(float *matrix, float angle);
void Vector3GetRotateYMatrix(float *matrix, float angle);
void Vector3GetRotateZMatrix(float *matrix, float angle);

int main(){
    int *Voxels = (int*)malloc(sizeof(int)*SPACE_WIDTH*SPACE_HEIGHT*SPACE_LENGTH);
    float *RGBAs = (float*)malloc(sizeof(float)*SPACE_WIDTH*SPACE_HEIGHT*SPACE_LENGTH*4);
    float *Pixels = (float*)malloc(sizeof(float)*SCREEN_WIDTH*SCREEN_HEIGHT*4);
    GenerateVoxels(Voxels);
    GenerateRBGAs(Voxels, RGBAs);
    
    float pos[3] = {0, 100, 100}; // 相机位置
    float dir[3] = {1.0f, 0, 0}; // 相机朝向
    float planeX[3] = {0, 0, 80}; // 相机平面垂直于dir
    float planeY[3] = {0, 60, 0}; // 其X分量和Y分量也互相垂直

    RayCasting(pos, dir, planeX, planeY, RGBAs, Pixels);
    free(Voxels);
    
    // 显示像素
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL 初始化失败：" << SDL_GetError() << endl;
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow(
        "RayCasting - SDL2 Display",    // 窗口标题
        SDL_WINDOWPOS_CENTERED,         // 窗口X位置（居中）
        SDL_WINDOWPOS_CENTERED,         // 窗口Y位置（居中）
        SCREEN_WIDTH,                   // 宽度
        SCREEN_HEIGHT,                  // 高度
        SDL_WINDOW_SHOWN                // 显示窗口
    );
    if (!window) {
        cerr << "窗口创建失败：" << SDL_GetError() << endl;
        free(RGBAs);
        free(Pixels);
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,                              // 自动选择渲染驱动
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer) {
        cerr << "渲染器创建失败：" << SDL_GetError() << endl;
        free(RGBAs);
        free(Pixels);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_TARGET,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    if (!texture){
        cerr << "纹理创建失败：" << SDL_GetError() << endl;
        free(RGBAs);
        free(Pixels);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // 转换成sdl像素格式
    Uint8 *sdl_pixels = (Uint8*)malloc(sizeof(Uint8) * SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    TransPixels(sdl_pixels, Pixels);

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // 更新纹理数据（将sdl_pixels写入纹理）
        SDL_UpdateTexture(
            texture,
            NULL,               // 更新整个纹理（NULL=全屏）
            sdl_pixels,         // 像素数据缓冲区
            SCREEN_WIDTH * 4    // 每行字节数（4字节/像素 × 宽度）
        );

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }


    free(sdl_pixels);
    free(RGBAs);
    free(Pixels);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
* @brief 为每个体素设置类型
* @param Voxels 体素指针
**/
void GenerateVoxels(int *Voxels){
    for (int i = 0; i < SPACE_WIDTH; ++i){
        for (int j = 0; j < SPACE_HEIGHT; ++j){
            for (int k = 0; k < SPACE_LENGTH; ++k){
                int idx = i*SPACE_HEIGHT*SPACE_LENGTH+j*SPACE_LENGTH+k;
                Voxels[idx] = Air;
            }
        }
    }
    GenerateCube(50,50,50,100,Voxels,Cube_Blue);
    GenerateSphere(100,100,100,30,Voxels,Sphere_Green);
    GenerateCube(90,90,90,20,Voxels,Cube_Red);
}

/**
* @brief 生成正方体体素
* @param x 起始x坐标
* @param y 起始y坐标
* @param z 起始z坐标
* @param d 边长
* @param Voxels 体素指针
* @param type 体素类型
**/
void GenerateCube(int x, int y, int z, int d, int *Voxels, VoxelType type){
    for (int i = x; i < d+x; ++i){
        for (int j = y; j < d+x; ++j){
            for (int k = z; k < d+z; ++k){
                if (i < 0 || i >= SPACE_WIDTH || j < 0 || j >= SPACE_HEIGHT || k < 0 || k >= SPACE_WIDTH) continue;
                int idx = i*SPACE_HEIGHT*SPACE_LENGTH+j*SPACE_LENGTH+k;
                Voxels[idx] = type;
            }
        }
    }
}

/**
* @brief 生成球体体素
* @param x 球心x坐标
* @param y 球心y坐标
* @param z 球心z坐标
* @param d 半径
* @param Voxels 体素指针
* @param type 体素类型
**/
void GenerateSphere(int x, int y, int z, int r, int *Voxels, VoxelType type){
    int r2 = r*r;
    for (int i = x-r; i < x+r; ++i){
        for (int j = y-r; j < y+r; ++j){
            for (int k = z-r; k < z+r; ++k){
                if (i < 0 || i >= SPACE_WIDTH || j < 0 || j >= SPACE_HEIGHT || k < 0 || k >= SPACE_WIDTH) continue;
                if ((i-x)*(i-x) + (j-y)*(j-y) + (k-z)*(k-z)<= r2){
                    int idx = i*SPACE_HEIGHT*SPACE_LENGTH+j*SPACE_LENGTH+k;
                    Voxels[idx] = type;
                }
            }
        }
    }
}

/**
* @brief 根据体素类型生成颜色信息
* @param Voxels 体素指针
* @param RGBAs 颜色信息指针
**/
void GenerateRBGAs(int *Voxels, float *RGBAs){
    for (int i = 0; i < SPACE_WIDTH; ++i){
        for (int j = 0; j < SPACE_HEIGHT; ++j){
            for (int k = 0; k < SPACE_LENGTH; ++k){
                int idx = i*SPACE_HEIGHT*SPACE_LENGTH+j*SPACE_LENGTH+k;
                int idx_rgba = i*SPACE_HEIGHT*SPACE_LENGTH*4+j*SPACE_LENGTH*4+k*4;
                switch (Voxels[idx]){
                case Cube_Blue:
                    RGBAs[idx_rgba] = 0;
                    RGBAs[idx_rgba + 1] = 0;
                    RGBAs[idx_rgba + 2] = 1.0f;
                    RGBAs[idx_rgba + 3] = 0.005f;
                    break;
                case Sphere_Green:
                    RGBAs[idx_rgba] = 0;
                    RGBAs[idx_rgba + 1] = 1.0f;
                    RGBAs[idx_rgba + 2] = 0;
                    RGBAs[idx_rgba + 3] = 0.01f;
                    break;
                case Cube_Red:
                    RGBAs[idx_rgba] = 1.0f;
                    RGBAs[idx_rgba + 1] = 0;
                    RGBAs[idx_rgba + 2] = 0;
                    RGBAs[idx_rgba + 3] = 0.02f;
                    break;
                default:
                    RGBAs[idx_rgba] = 0;
                    RGBAs[idx_rgba + 1] = 0;
                    RGBAs[idx_rgba + 2] = 0;
                    RGBAs[idx_rgba + 3] = 0;
                }
            }
        }
    }
}

/**
* @brief raycasting生成像素颜色信息
* @param pos 相机坐标
* @param dir 相机朝向
* @param planeX 相机平面x分量
* @param planeY 相机平面y分量
* @param RGBAs 输入三维体素颜色信息
* @param Pixels 返回平面像素颜色信息
**/
void RayCasting(float *pos, float *dir, float *planeX, float *planeY, float *RGBAs, float *Pixels){
    // 获取Pixels[i][j]的rgba
    for (int j = 0; j < SCREEN_HEIGHT; ++j){
        for (int i = 0; i < SCREEN_WIDTH; ++i){
            // 计算光线的出发点
            float rayPos[3];
            float mapY = 2.0f * (SCREEN_HEIGHT - 1.0f - j) / SCREEN_HEIGHT - 1.0f; // 映射到[-1, 1)，同时翻转y轴
            float mapX = 2.0f * i / SCREEN_WIDTH - 1.0f;
            float temp[3];
            Vector3Scale(temp, planeX, mapX);
            Vector3Copy(rayPos, temp);
            Vector3Scale(temp, planeY, mapY);
            Vector3Add(rayPos, rayPos, temp);
            Vector3Add(rayPos, rayPos, dir);
            Vector3Add(rayPos, rayPos, pos);
            // 从前向后
            float samplePos[3];
            float step[3];
            float underColor[4] = {0, 0, 0, 0}; // 初始颜色
            Vector3Normalize(step, dir);
            // Vector3Scale(step, 0.5);
            int t = 0;
            while(!PosValid(samplePos[0], samplePos[1], samplePos[2]) && t < 100){
                Vector3Add(samplePos, rayPos, step);
                t++;
            }
            while (PosValid(samplePos[0], samplePos[1], samplePos[2])){
                // 三线性插值
                float rgba[4];
                TriInterpolation(rgba, samplePos, RGBAs);
                if (rgba[3] > 0.001f){
                    // 合成颜色
                    for (int l = 0; l < 3; ++l){
                        underColor[l] = underColor[l] + (1-underColor[3]) * rgba[l] * rgba[3];
                    }
                    // 计算不透明度
                    underColor[3] = underColor[3] + (1-underColor[3])*rgba[3];

                    if (underColor[3] >= 0.99f){
                        underColor[3] = 1.0f;
                        break;
                    }
                }
                Vector3Add(samplePos, samplePos, step);
            }
            int idx = (j * SCREEN_WIDTH + i) * 4;
            for (int k = 0; k < 4; ++k){
                Pixels[idx+k] = underColor[k];
            }
        }
    }
}

/**
* @brief 判断坐标是否在空间内
* @param x x轴
* @param y y轴
* @param z z轴
* @return 坐标是否有效
**/
int PosValid(float x, float y, float z){
    return x >= 0 && x < SPACE_WIDTH && y >= 0 && y < SPACE_HEIGHT && z >= 0 && z < SPACE_LENGTH;
}

/**
* @brief 三线性插值计算采样点的rgba信息
* @param rgba 返回值指针
* @param pos 采样点坐标
* @param RGBAs 体素信息指针
**/
void TriInterpolation(float *rgba, float *pos, float *RGBAs){
    float sampleX = pos[0], sampleY = pos[1], sampleZ = pos[2];
    int x0 = (int)sampleX, y0 = (int)sampleY, z0 = (int)sampleZ;
    if (x0 < 0 || x0 >= SPACE_WIDTH-1 ||
        y0 < 0 || y0 >= SPACE_HEIGHT-1 ||
        z0 < 0 || z0 >= SPACE_LENGTH-1) {
        rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0;
        return;
    }
    float dx = sampleX - x0;    // x方向偏移[0,1)
    float dy = sampleY - y0;    // y方向偏移[0,1)
    float dz = sampleZ - z0;    // z方向偏移[0,1)
    float dx1 = 1.0f - dx;      // 1 - dx
    float dy1 = 1.0f - dy;      // 1 - dy
    float dz1 = 1.0f - dz;      // 1 - dz
    float *v000 = &(R_AT(RGBAs, x0,   y0,   z0  )); // (x0,y0,z0)
    float *v100 = &(R_AT(RGBAs, x0+1, y0,   z0  )); // (x0+1,y0,z0)
    float *v010 = &(R_AT(RGBAs, x0,   y0+1, z0  )); // (x0,y0+1,z0)
    float *v110 = &(R_AT(RGBAs, x0+1, y0+1, z0  )); // (x0+1,y0+1,z0)
    float *v001 = &(R_AT(RGBAs, x0,   y0,   z0+1)); // (x0,y0,z0+1)
    float *v101 = &(R_AT(RGBAs, x0+1, y0,   z0+1)); // (x0+1,y0,z0+1)
    float *v011 = &(R_AT(RGBAs, x0,   y0+1, z0+1)); // (x0,y0+1,z0+1)
    float *v111 = &(R_AT(RGBAs, x0+1, y0+1, z0+1)); // (x0+1,y0+1,z0+1)

    for (int i = 0; i < 4; ++i){
        rgba[i] = v000[i] * dx1 * dy1 * dz1  // (x0,y0,z0)权重
                + v100[i] * dx  * dy1 * dz1  // (x0+1,y0,z0)权重
                + v010[i] * dx1 * dy  * dz1  // (x0,y0+1,z0)权重
                + v110[i] * dx  * dy  * dz1  // (x0+1,y0+1,z0)权重
                + v001[i] * dx1 * dy1 * dz   // (x0,y0,z0+1)权重
                + v101[i] * dx  * dy1 * dz   // (x0+1,y0,z0+1)权重
                + v011[i] * dx1 * dy  * dz   // (x0,y0+1,z0+1)权重
                + v111[i] * dx  * dy  * dz;  // (x0+1,y0+1,z0+1)权重
    }
}

/**
* @brief 像素格式转换成sdl texture支持的
* @param sdl_pixels sdl像素指针
* @param Pixels 原像素指针rgba
**/
void TransPixels(Uint8 *sdl_pixels, float *Pixels){
    for (int y_sdl = 0; y_sdl < SCREEN_HEIGHT; y_sdl++) {
        for (int x_sdl = 0; x_sdl < SCREEN_WIDTH; x_sdl++) {
            int idx_sdl = (y_sdl * SCREEN_WIDTH + x_sdl) * 4;
            int idx_src = (y_sdl * SCREEN_WIDTH + x_sdl) * 4; // 直接对应
    
            float r = Pixels[idx_src];
            float g = Pixels[idx_src+1];
            float b = Pixels[idx_src+2];
            float a = Pixels[idx_src+3];
    
            sdl_pixels[idx_sdl]     = (Uint8)(fminf(fmaxf(b, 0.0f), 1.0f) * 255.0f);
            sdl_pixels[idx_sdl + 1] = (Uint8)(fminf(fmaxf(g, 0.0f), 1.0f) * 255.0f);
            sdl_pixels[idx_sdl + 2] = (Uint8)(fminf(fmaxf(r, 0.0f), 1.0f) * 255.0f);
            sdl_pixels[idx_sdl + 3] = (Uint8)(fminf(fmaxf(a, 0.0f), 1.0f) * 255.0f);
        }
    }
}

void Vector3Copy(float *dest, float *src){
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

void Vector3Add(float *dest, float *src1, float *src2){
    dest[0] = src1[0] + src2[0];
    dest[1] = src1[1] + src2[1];
    dest[2] = src1[2] + src2[2];
}

void Vector3Scale(float* dest, float *vector, float scalar){
    dest[0] = vector[0] * scalar;
    dest[1] = vector[1] * scalar;
    dest[2] = vector[2] * scalar;
}

void Vector3Normalize(float *dest, float *src){
    float len = sqrt(src[0]*src[0] + src[1]*src[1] + src[2]*src[2]);
    if (len == 0) len = 1.0f;
    Vector3Scale(dest, src, 1/len);
}

void Vector3Rotate(float *dest, float *src, float *matrix){
    float x = src[0];
    float y = src[1];
    float z = src[2];
    dest[0] = matrix[0] * x + matrix[1] * y + matrix[2] * z;  // 第一行 × 源向量
    dest[1] = matrix[3] * x + matrix[4] * y + matrix[5] * z;  // 第二行 × 源向量
    dest[2] = matrix[6] * x + matrix[7] * y + matrix[8] * z;  // 第三行 × 源向量
}

void Vector3GetRotateXMatrix(float *matrix, float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    matrix[0] = 1.0f; matrix[1] = 0.0f;  matrix[2] = 0.0f;
    matrix[3] = 0.0f; matrix[4] = cosA;  matrix[5] = -sinA;
    matrix[6] = 0.0f; matrix[7] = sinA;  matrix[8] = cosA;
}

void Vector3GetRotateYMatrix(float *matrix, float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    matrix[0] = cosA;  matrix[1] = 0.0f; matrix[2] = sinA;
    matrix[3] = 0.0f;  matrix[4] = 1.0f; matrix[5] = 0.0f;
    matrix[6] = -sinA; matrix[7] = 0.0f; matrix[8] = cosA;
}

void Vector3GetRotateZMatrix(float *matrix, float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    matrix[0] = cosA;  matrix[1] = -sinA; matrix[2] = 0.0f;
    matrix[3] = sinA;  matrix[4] = cosA;  matrix[5] = 0.0f;
    matrix[6] = 0.0f;  matrix[7] = 0.0f;  matrix[8] = 1.0f;
}