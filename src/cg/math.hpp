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


template <typename T>
struct Vec2 {
    T x = 0, y = 0;

    // Construtores
    constexpr Vec2() = default;
    constexpr Vec2(T x, T y) : x(x), y(y) {}

    // Construtor de conversão (somente quando U→T é válido):
    template<std::convertible_to<T> U>
    Vec2(Vec2<U> const& v) : x(v.x) {}
    // De forma similar para o operador de assinatura:
    template<std::convertible_to<T> U>
    Vec2& operator=(Vec2<U> const& v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    // Operadores aritméticos
    template <typename S>
    constexpr Vec2 operator/(S f) const { return {x / f, y / f}; }
    template <typename S>
    constexpr Vec2 operator*(S f) const { return {x * f, y * f}; }

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

    inline T dot() {
        return x * x + y * y;
    }
};

using Vector2 = Vec2<float>;
using Vector2i = Vec2<int>;


template <typename T>
struct Vec3 {
    T x = 0.0f, y = 0.0f, z = 0.0f;

    // Construtores
    constexpr Vec3() = default;
    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    // Construtor de conversão (somente quando U→T é válido):
    template<std::convertible_to<T> U>
    Vec3(Vec3<U> const& v) : x(v.x) {}
    // De forma similar para o operador de assinatura:
    template<std::convertible_to<T> U>
    Vec3& operator=(Vec3<U> const& v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    // Operadores aritméticos
    template <typename S>
    Vec3 operator/(S f) const { return {x / f, y / f, z / f}; }

    template <typename S>
    Vec3 operator*(S f) const { return {x * f, y * f, z * f}; }

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

using Vector3 = Vec3<float>;
using Vector3i = Vec3<int>;


struct ColorRgb {
    unsigned char r = 0, g = 0, b = 0;

    ColorRgb() = default; // implicit constructor
    ColorRgb(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
    Vector3 normalized() const { return Vector3(r / 255.0f, g / 255.0f, b / 255.0f); }
};


struct Color {
    float r, g, b, a;

    Color(ColorRgb color, float alpha) : r{(float)color.r}, g{(float)color.g}, b{(float)color.b}, a{(float)alpha} {}
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
