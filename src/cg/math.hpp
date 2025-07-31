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
    union {
        struct {
            T x = 0, y = 0;
        };
        T coords[2];
        T rows[2];
    };

    // Construtores
    constexpr Vec2() = default;
    constexpr Vec2(T x, T y) : x(x), y(y) {}

    // Construtor de conversão (somente quando U→T é válido):
    template<std::convertible_to<T> U>
    constexpr Vec2(Vec2<U> const& v) : x(v.x) {}
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
    union {
        struct {
            T x = 0.0f, y = 0.0f, z = 0.0f;
        };
        T coords[3];
        T rows[3];
    };

    // Construtores
    constexpr Vec3() = default;
    constexpr Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
    constexpr Vec3(Vec2<T> v, T z = 0) : x(v.x), y(v.y), z(z) {}

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


template <typename T>
struct Mat3 /* Matriz column-major 3x3. Representing a 2D transformation. */
{
    Vec3<T>columns[3];

    constexpr Mat3() = default;
    constexpr Mat3(Vec3<T> col_v1, Vec3<T> col_v2, Vec3<T> col_v3) : columns( col_v1, col_v2, col_v3 ) {}
    constexpr Mat3(Vec2<T> translation) : columns( {1, 0, 0}, {0, 1, 0}, {translation, 1}) {}
    constexpr Mat3(Vec2<T> v1_xaxis, Vec2<T> v2_yaxis) : columns( Vec3<T>{v1_xaxis.x}, Vec3<T>{v2_yaxis.x}, Vec3<T>{0, 0, 1}) {}
    constexpr Mat3(T x1_x, T x2_y, T x3_h, T y1_x, T y2_y, T y3_h, T h1_x, T h2_y, T h3_h) : columns{ x1_x, x2_y, x3_h, y1_x, y2_y, y3_h, h1_x, h2_y, h3_h } {}

    static constexpr inline Mat3 translation(Vec2 direction, Mat3 m) {
        return {
            Vec3{ 1, 0, 0 },
            Vec3{ 0, 1, 0 },
            Vec3{ direction.x, direction.y, 1 },
        } * m;
    }
    constexpr inline Mat3& translate(Vec2 direction) {
        return *this *= {
            Vec3{ 1, 0, 0 },
            Vec3{ 0, 1, 0 },
            Vec3{ direction.x, direction.y, 1 },
        };
    }
    static constexpr inline Mat3& rotation(float angle, Mat3 m) {
        return {
            Vec3{ cosf(angle), sinf(theta), 0 },
            Vec3{ -sinf(angle), cosf(theta), 0 },
            Vec3{ 0, 0, 1 },
        } * m;
    }
    constexpr inline Mat3& rotate(float angle) {
        return *this *= {
            Vec3{ cosf(angle), sinf(theta), 0 },
            Vec3{ -sinf(angle), cosf(theta), 0 },
            Vec3{ 0, 0, 1 },
        };
    }

    constexpr inline const T get(std::size_t col, std::size_t row) const {
        return columns[col].rows[row];
    }

    constexpr inline T& get(std::size_t col, std::size_t row) {
        return columns[col].rows[row];
    }

    constexpr inline T& set(std::size_t col, std::size_t row, T value) {
        return columns[col].rows[row] = value;
    }

};


// Point transformation.
template<typename T, std::convertible_to<T> U>
inline Vec3<std::common_type<T, U>> operator*(const Mat3<T>& m, const Vec3<U>& v) {
    return {
        m.columns[0].x * v.x + m.columns[1].x * v.y + m.columns[2].x * v.z,
        m.columns[0].y * v.x + m.columns[1].y * v.y + m.columns[2].y * v.z,
        m.columns[0].z * v.x + m.columns[1].z * v.y + m.columns[2].z * v.z
    };
}


template <typename T, std::convertible_to<T> U>
inline Mat3<T> operator*(Mat3<T> a, Mat3<U> b) {
    m.columns[0] = {a * b.columns[0]};
    m.columns[1] = {a * b.columns[1]};
    m.columns[2] = {a * b.columns[2]};
    return a;
}


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

using Transform2D = Mat3<float>;
using Matrix3i = Mat3<int>;

} // namespace cg
