#pragma once

#include <api.hpp>

// Windows Macros may override later std::min / std::max uses
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif


// Inclui algumas funções matemáticas básicas
#include <random>
#include <numbers>

namespace cg
{

template <typename T>
constexpr T TAU = std::numbers::pi_v<T> * static_cast<T>(2);

template <typename T>
constexpr T PI = std::numbers::pi_v<T>;


struct Vector2 {
    float x = 0.0f, y = 0.0f;

    constexpr Vector2() = default;
    // Construtores
    constexpr Vector2(float x, float y) : x(x), y(y) {}

    // Operadores aritméticos
    constexpr Vector2 operator/(float f) const { return {x / f, y / f}; }
    constexpr Vector2 operator*(float f) const { return {x * f, y * f}; }
    constexpr Vector2 operator+(Vector2 v) const { return {x + v.x, y + v.y}; }
    constexpr Vector2 operator-(Vector2 v) const { return {x - v.x, y - v.y}; }
    Vector2& operator+=(Vector2 v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vector2& operator-=(Vector2 v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline float dot() {
        return x * x + y * y;
    }
};


struct Vector3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    // Construtores
    constexpr Vector3() = default;
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Operadores aritméticos
    Vector3 operator/(float f) const { return {x / f, y / f, z / f}; }
    Vector3 operator*(float f) const { return {x * f, y * f, z * f}; }
    Vector3 operator+(Vector3 v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vector3 operator-(Vector3 v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vector3& operator+=(const Vector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vector3& operator-=(const Vector3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    inline float dot() {
        return x * x + y * y + z * z;
    }
    inline Vector3 cross() {
        return Vector3(y * z, z * x, x * y);
    }
};


struct Color {
    float r, g, b, a;
};

struct ColorRgb {
    unsigned char r = 0, g = 0, b = 0;

    ColorRgb() = default; // implicit constructor
    ColorRgb(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
    Vector3 normalized() const { return Vector3(r / 255.0f, g / 255.0f, b / 255.0f); }
};


inline void randomizeColor()
{
    glColor3ub(
        (std::rand() % 256),
        (std::rand() % 256),
        (std::rand() % 256)
    );
}

} // namespace cg
