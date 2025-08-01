#pragma once

#include <util.hpp>

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

// TODO -> default tau/pi precision to float
template <typename T>
constexpr T TAU = std::numbers::pi_v<T> * static_cast<T>(2);

template <typename T>
constexpr T PI = std::numbers::pi_v<T>;

using Angle = float; // Angle in radians.


template <typename T>
struct Vec2 {
    T x = 0, y = 0;

    // Construtores
    constexpr Vec2() = default;
    constexpr Vec2(T x, T y) : x(x), y(y) {}

    // Construtor de conversão (somente quando U→T é válido):
    template<std::convertible_to<T> U>
    constexpr Vec2(Vec2<U> const& v) : x(v.x), y(v.y) {}
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

    constexpr T& operator[](size_t i) {
        assert_err(i < 2, "Index out of range.");
        return *(&x + i);
    }

    constexpr T operator[](size_t i) const {
        assert_err(i < 2, "Index out of range.");
        return *(&x + i);
    }

    constexpr inline T dot() {
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
    constexpr Vec3 operator/(S f) const { return {x / f, y / f, z / f}; }

    template <typename S>
    constexpr Vec3 operator*(S f) const { return {x * f, y * f, z * f}; }

    constexpr Vec3 operator+(Vec3 v) const { return {x + v.x, y + v.y, z + v.z}; }
    constexpr Vec3 operator-(Vec3 v) const { return {x - v.x, y - v.y, z - v.z}; }
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

    constexpr T& operator[](size_t i) {
        assert_err(i < 3, "Index out of range.");
        return *(&x + i);
    }

    constexpr T operator[](size_t i) const {
        assert_err(i < 3, "Index out of range.");
        return *(&x + i);
    }

    constexpr inline float dot() {
        return x * x + y * y + z * z;
    }
    constexpr inline Vec3 cross() {
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
    constexpr Mat3(Vec3<T> col_v1, Vec3<T> col_v2, Vec3<T> col_v3) : columns{ col_v1, col_v2, col_v3 } {}
    constexpr Mat3(Vec2<T> translation) : columns{ {1, 0, 0}, {0, 1, 0}, {translation, 1} } {}
    constexpr Mat3(Vec2<T> v1_xaxis, Vec2<T> v2_yaxis) : columns{ Vec3<T>{v1_xaxis, 1.0f}, Vec3<T>{v2_yaxis, 1.0f}, Vec3<T>{0, 0, 1} } {}
    constexpr Mat3(T x1_x, T x2_y, T x3_h, T y1_x, T y2_y, T y3_h, T h1_x, T h2_y, T h3_h) : columns{
        { x1_x, x2_y, x3_h },
        { y1_x, y2_y, y3_h },
        { h1_x, h2_y, h3_h }
    } {}

    constexpr Mat3& operator*=(Mat3<T> m) {
        columns[0] *= m;
        columns[1] *= m;
        columns[2] *= m;
        return *this;
    }

    constexpr Mat3(Angle theta) : columns(
        Vec3<T>{ cosf(theta), sinf(theta), 0 },
        Vec3<T>{ -sinf(theta), cosf(theta), 0 },
        Vec3<T>{ 0, 0, 1 }
    ) {}
    constexpr inline Mat3& translate(Vec2<T> direction) {
        return *this *= Mat3<T>{direction};
    }
    constexpr inline Mat3& rotate(float angle) {
        return *this *= Mat3{angle};
    }

    constexpr inline Vec2<T> transform(std::convertible_to<T> auto x, std::convertible_to<T> auto y) const {
        return {
            columns[0].x * x + columns[1].x * y + columns[2].x,
            columns[0].y * x + columns[1].y * y + columns[2].y
        };
    }
    constexpr inline Vec2<T> transform(std::convertible_to<Vec2<T>> auto v) const {
        return (*this).transform(v.x, v.y);
    }

    constexpr inline const T get(std::size_t col, std::size_t row) const {
        assert_err(col < 3 && row < 3, "Index out of range.");
        return columns[col][row];
    }

    constexpr inline T& get(std::size_t col, std::size_t row) {
        assert_err(col < 3 && row < 3, "Index out of range.");
        return columns[col][row];
    }

    constexpr inline T& set(std::size_t col, std::size_t row, T value) {
        assert_err(col < 3 && row < 3, "Index out of range.");
        return columns[col][row] = value;
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

template<typename T, std::convertible_to<T> U>
inline auto& operator*=(const Vec3<U>& v, const Mat3<T>& m) {
    v.x = m.columns[0].x * v.x + m.columns[1].x * v.y + m.columns[2].x * v.z;
    v.y = m.columns[0].y * v.x + m.columns[1].y * v.y + m.columns[2].y * v.z;
    v.z = m.columns[0].z * v.x + m.columns[1].z * v.y + m.columns[2].z * v.z;
    return v;
}


template <typename T, std::convertible_to<T> U>
inline Mat3<T> operator*(Mat3<T> a, Mat3<U> b) {
    a.columns[0] = {a * b.columns[0]};
    a.columns[1] = {a * b.columns[1]};
    a.columns[2] = {a * b.columns[2]};
    return a;
}


struct ColorRgb {
    unsigned char r = 0, g = 0, b = 0;

    constexpr ColorRgb() = default; // implicit constructor
    constexpr ColorRgb(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
    inline Vector3 normalized() const { return Vector3(r / 255.0f, g / 255.0f, b / 255.0f); }
};


struct Color {
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

    constexpr Color() = default;
    constexpr Color(float red, float green, float blue, float alpha = 1.0f) : r(red), g(green), b(blue), a(alpha) {}
    constexpr Color(ColorRgb color, float alpha = 1.0f) : r{(float)color.r}, g{(float)color.g}, b{(float)color.b}, a{(float)alpha} {}
    constexpr Color(Vector3 v, float alpha = 1.0f) : r(v.x), g(v.y), b(v.z), a(alpha) {}

    constexpr operator ColorRgb() {
        return {
            (unsigned char)(r * 255),
            (unsigned char)(g * 255),
            (unsigned char)(b * 255),
        };
    }
};

namespace colors {
    constexpr Color NONE {};
    constexpr Color BLACK { 0.0f, 0.0f, 0.0f };
    constexpr Color WHITE { 1.0f, 1.0f, 1.0f };
    constexpr Color RED   { 1.0f, 0.0f, 0.0f };
    constexpr Color GREEN { 0.0f, 1.0f, 0.0f };
    constexpr Color BLUE  { 0.0f, 0.0f, 1.0f };
    constexpr ColorRgb RGB_BLACK { 0, 0, 0 };
    constexpr ColorRgb RGB_WHITE { 1, 1, 1 };
    constexpr ColorRgb RGB_RED   { 1, 0, 0 };
    constexpr ColorRgb RGB_GREEN { 0, 1, 0 };
    constexpr ColorRgb RGB_BLUE  { 0, 0, 1 };

    inline void glRandomize()
    {
        glColor3ub(
            (std::rand() % 256),
            (std::rand() % 256),
            (std::rand() % 256)
        );
    }
}

using Transform2D = Mat3<float>;
} // namespace cg
