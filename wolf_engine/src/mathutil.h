#ifndef MATHUTIL_H
#define MATHUTIL_H

#define TRIG_LUT_SIZE  4096
#define TRIG_LUT_MASK  4095
#define PI             3.14159265f

#include <cmath>
#include <array>

inline float fcos(int index){
    static const auto cos_lut = [](){
        std::array<float, TRIG_LUT_SIZE> arr;
        const float radPerIndex = 0.00153398f;        // PI * 2 / 4096
        for(int i = 0; i < TRIG_LUT_SIZE; i++){
            arr[i] = std::cosf(i * radPerIndex); 
        }
        return arr;
    }();
    return cos_lut[index & TRIG_LUT_MASK]; 
}

inline float fsin(int index){
    return fcos(index - 1024);
}

struct Vector2 {
    float x, y;

    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float a, float b) : x(a), y(b) {}

    Vector2 operator+(Vector2 other) {
        return Vector2(x + other.x, y + other.y);
    }
    Vector2 operator-(Vector2 other) {
        return Vector2(x - other.x, y - other.y);
    }
    Vector2 operator*(float scaler) {
        return Vector2(x * scaler, y * scaler);
    }
    Vector2 operator/(float scaler) {
        return (scaler != 0.0f) ? Vector2(x / scaler, y / scaler) : Vector2(0.0f, 0.0f);
    }
    Vector2 Normal() {
        return Vector2(-y, x);
    }
    float Dot(Vector2 other) {
        return x * other.x + y * other.y;
    }
};

#endif
