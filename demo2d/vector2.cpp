#include "vector2.h"

Vector2 Vector2Add(Vector2 a, Vector2 b) {
    return {a.x + b.x, a.y + b.y};
}


Vector2 Vector2Subtract(Vector2 a, Vector2 b) {
    return {a.x - b.x, a.y - b.y};
}


Vector2 Vector2MultiplyScalar(Vector2 v, float scalar) {
    return {v.x * scalar, v.y * scalar};
}


Vector2 Vector2Negate(Vector2 v) {
    return {-v.x, -v.y};
}


float Vector2DotProduct(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}


float Vector2CrossProduct(Vector2 a, Vector2 b) {
    return a.x * b.y - a.y * b.x;
}


float Vector2Length(Vector2 v) {
    return sqrtf(v.x * v.x + v.y * v.y); // sqrtf 是float版sqrt，效率更高
}


float Vector2LengthSquared(Vector2 v) {
    return v.x * v.x + v.y * v.y;
}


Vector2 Vector2Normalize(Vector2 v) {
    float len = Vector2Length(v);
    if (len < 1e-6) { // 避免除以0（浮点精度容错）
        return {0.0f, 0.0f};
    }
    return {v.x / len, v.y / len};
}


float Vector2Distance(Vector2 a, Vector2 b) {
    return Vector2Length(Vector2Subtract(a, b));
}


float Vector2DistanceSquared(Vector2 a, Vector2 b) {
    return Vector2LengthSquared(Vector2Subtract(a, b));
}


float Vector2Angle(Vector2 v) {
    return atan2f(v.y, v.x); // atan2f 是float版atan2
}


Vector2 Vector2Rotate(Vector2 v, float angleRad) {
    float cosA = cosf(angleRad);
    float sinA = sinf(angleRad);
    // 旋转矩阵：[cosθ, -sinθ; sinθ, cosθ]
    return {
        v.x * cosA - v.y * sinA,
        v.x * sinA + v.y * cosA
    };
}


Vector2 Vector2Project(Vector2 a, Vector2 b) {
    float bLenSq = Vector2LengthSquared(b);
    if (bLenSq < 1e-6) {
        return {0.0f, 0.0f};
    }
    float scale = Vector2DotProduct(a, b) / bLenSq;
    return Vector2MultiplyScalar(b, scale);
}