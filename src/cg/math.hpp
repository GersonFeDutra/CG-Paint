#pragma once

#include <random>
#include <numbers>

// Inclui algumas funções matemáticas básicas
#include "../api.hpp"

namespace cg
{

template <typename T>
constexpr T TAU = std::numbers::pi_v<T> * static_cast<T>(2);

template <typename T>
constexpr T PI = std::numbers::pi_v<T>;


struct Vec2 {
    float x = 0.0f, y = 0.0f;

    constexpr Vec2() = default;
    // Construtores
    constexpr Vec2(float x, float y) : x(x), y(y) {}

    // Operadores aritméticos
    constexpr Vec2 operator/(float f) const { return {x / f, y / f}; }
    constexpr Vec2 operator*(float f) const { return {x * f, y * f}; }
    constexpr Vec2 operator+(Vec2 v) const { return {x + v.x, y + v.y}; }
    constexpr Vec2 operator-(Vec2 v) const { return {x - v.x, y - v.y}; }
    Vec2& operator+=(Vec2 v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vec2& operator-=(Vec2 v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    inline float dot() {
        return x * x + y * y;
    }
};


struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    // Construtores
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Operadores aritméticos
    Vec3 operator/(float f) const { return {x / f, y / f, z / f}; }
    Vec3 operator*(float f) const { return {x * f, y * f, z * f}; }
    Vec3 operator+(Vec3 v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator-(Vec3 v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3& operator+=(const Vec3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vec3& operator-=(const Vec3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    inline float dot() {
        return x * x + y * y + z * z;
    }
    inline Vec3 cross() {
        return Vec3(y * z, z * x, x * y);
    }
};


struct Color {
    unsigned char r, g, b;

    Color() : r(0), g(0), b(0) {} // implicit constructor
    Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
    Vec3 normalized() const { return Vec3(r / 255.0f, g / 255.0f, b / 255.0f); }
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
