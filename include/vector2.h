#pragma once

#include <cmath>
#include <iostream>

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 600;
const int MAP_OP_X = 800;
const int MAP_OP_Y = 0;
const int MAP_WIDTH = 800;
const int MAP_HEIGHT = 600;

typedef struct Vector2 {
    float x, y;
} Vector2;

Vector2 Vector2Add(Vector2 a, Vector2 b);


Vector2 Vector2Subtract(Vector2 a, Vector2 b);


Vector2 Vector2MultiplyScalar(Vector2 v, float scalar);


Vector2 Vector2Negate(Vector2 v);


float Vector2DotProduct(Vector2 a, Vector2 b);


float Vector2CrossProduct(Vector2 a, Vector2 b);

float Vector2Length(Vector2 v);


float Vector2LengthSquared(Vector2 v);


Vector2 Vector2Normalize(Vector2 v);


float Vector2Distance(Vector2 a, Vector2 b);


float Vector2DistanceSquared(Vector2 a, Vector2 b);

float Vector2Angle(Vector2 v);


Vector2 Vector2Rotate(Vector2 v, float angleRad);


Vector2 Vector2Project(Vector2 a, Vector2 b);