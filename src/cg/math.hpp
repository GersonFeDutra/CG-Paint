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
#include <type_traits>
#include <iostream>
#include <fstream>


namespace cg
{

template<typename T, typename U>
using LargerType = typename std::common_type<T, U>::type;

// TODO -> default tau/pi precision to float
template <typename T>
constexpr T TAU = std::numbers::pi_v<T> * static_cast<T>(2);

template <typename T>
constexpr T PI = std::numbers::pi_v<T>;

using Angle = float; // Angle in radians.

template <typename T>
class Vec2Interpolator; // forward definition

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

    template <std::convertible_to<T> U>
    constexpr inline auto dot(Vec2<U> with) const {
        return x * with.x + y * with.y;
    }

    constexpr inline T norm() const {
        return sqrtf(dot(*this));
    }

    constexpr Vec2<T> normalized() const {
        return *this / norm();
    }

    constexpr inline T distance(Vec2<T> to) const {
        return (to - *this).norm();
    }

    constexpr inline Vec2<T> direction(Vec2<T> to) const {
        return (to - *this).normalized();
    }

    constexpr inline T length() const {
        return norm();
    }

    /* Get linear interpolation next step. */
    inline Vec2 lerp(Vec2 to, float by) const {
        return { x + (to.x - x) * by, y + (to.y - y) * by };
    }

    /* Returns a linear interpolation Iterator towards `to` by the given amount. */
    template <std::convertible_to<T> U>
    Vec2Interpolator<LargerType<T, U>> lerpIterator(Vec2<U> to, float by) const;

    friend std::ostream& operator<< <T>(std::ostream& os, const Vec2<T>& v);
    friend std::istream& operator>> <T>(std::istream& is, Vec2<T>& v);
    
    friend std::ofstream& operator<< <T>(std::ofstream& ofs, const Vec2<T>& v);
    friend std::ifstream& operator>> <T>(std::ifstream& ifs, Vec2<T>& v);
};

// Friends
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vec2<T>& v) {
    return os << '(' << v.x << ", " << v.y << ')';
}

template <typename T>
inline std::istream& operator>>(std::istream& is, Vec2<T>& v) {
    try {
        char c;
        if (!(is >> c >> v.x >> c >> v.y >> c)) // Read in the format (x, y)
            is.setstate(std::ios::failbit); // marca falha no stream
    }
    catch (...) {
        is.setstate(std::ios::failbit);
    }
    return is;
}
template <typename T>
inline std::ofstream& operator<<(std::ofstream& ofs, const Vec2<T>& v) {
    ofs << '(' << v.x << ", " << v.y << ')'; // Save in the format (x, y)
    return ofs;
}
template <typename T>
inline std::ifstream& operator>>(std::ifstream& ifs, Vec2<T>& v) {
    try {
        while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
			ifs.ignore(); // Ignore leading whitespace
        if (ifs.peek() == EOF) {
            ifs.setstate(std::ios::failbit);
            return ifs;
		}

        Vec2<T> parsed;
        // Load in the format (x, y)
        if (ifs.peek() == '(')
		    ifs.ignore(); // Ignore '('
        else
            ifs.setstate(std::ios::failbit);

        if (!(ifs >> parsed.x)) {
            ifs.setstate(std::ios::failbit); // marca falha no stream
            return ifs;
        }
        while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
            ifs.ignore(); // Ignore leading whitespace
        if (ifs.peek() == ',')
            ifs.ignore(); // Ignore ','
        else
            ifs.setstate(std::ios::failbit);

        if (!(ifs >> parsed.y)) {
            ifs.setstate(std::ios::failbit); // marca falha no stream
            return ifs;
		}
        while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
			ifs.ignore(); // Ignore leading whitespace
        if (ifs.peek() == ')')
            ifs.ignore(); // Ignore ')'
        else
			ifs.setstate(std::ios::failbit);
		v = parsed; // Assign the parsed value to the Vec2
    }
    catch (...) {
        ifs.setstate(std::ios::failbit);
    }
    return ifs;
}

// Operadores aritméticos
template <typename T, std::convertible_to<T> U>
constexpr Vec2<LargerType<T, U>> operator/(Vec2<T> v, U scalar) { return { v.x / scalar, v.y / scalar }; }

template <typename T, std::convertible_to<T> U>
constexpr Vec2<LargerType<T, U>> operator*(Vec2<T> v, U scalar) { return { v.x * scalar, v.y * scalar }; }

template <typename T, std::convertible_to<T> U>
constexpr Vec2<LargerType<T, U>> operator+(Vec2<T> u, Vec2<U> v) { return { u.x + v.x, u.y + v.y }; }

template <typename T, std::convertible_to<T> U>
constexpr Vec2<LargerType<T, U>> operator-(Vec2<T> u, Vec2<U> v) { return { u.x - v.x, u.y - v.y }; }

template <typename T, std::convertible_to<T> U>
constexpr bool operator==(Vec2<T> u, Vec2<U> v) { return u.x == v.x && u.y == v.y; }

template <typename T, std::convertible_to<T> U>
constexpr bool operator!=(Vec2<T> u, Vec2<U> v) { return !(u == v); }

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

    friend std::ostream& operator<< <T>(std::ostream& os, const Vec3<T>& v);
    friend std::istream& operator>> <T>(std::istream& is, Vec3<T>& v);

    friend std::ofstream& operator<< <T>(std::ofstream& ofs, const Vec3<T>& v);
    friend std::ifstream& operator>> <T>(std::ifstream& ifs, Vec3<T>& v);
};
// Friends
template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Vec3<T>& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}
template <typename T>
inline std::istream& operator>>(std::istream& is, Vec3<T>& v) {
    try {
        char c;
        if (!(is >> c >> v.x >> c >> v.y >> c >> v.z >> c)) // Read in the format (x, y, z)
            is.setstate(std::ios::failbit); // marca falha no stream
    }
    catch (...) {
        is.setstate(std::ios::failbit);
    }
    return is;
}
template <typename T>
inline std::ofstream& operator<<(std::ofstream& ofs, const Vec3<T>& v) {
    ofs << "(" << v.x << ", " << v.y << ", " << v.z << ")"; // Save in the format (x, y, z)
    return ofs;
}
template <typename T>
inline std::ifstream& operator>>(std::ifstream& ifs, Vec3<T>& v) {
    try {
        while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
            ifs.ignore(); // Ignore leading whitespace

        Vec3<T> parsed;
        // Load in the format (x, y)
        if (ifs.peek() == '(')
            ifs.ignore(); // Ignore '('
        else
            ifs.setstate(std::ios::failbit);

		// Read x
        if (!(ifs >> parsed.x)) {
            ifs.setstate(std::ios::failbit); // marca falha no stream
            return ifs;
        }

        while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
            ifs.ignore(); // Ignore leading whitespace
        if (ifs.peek() == ',')
            ifs.ignore(); // Ignore ','
        else
            ifs.setstate(std::ios::failbit);

		// Read y
        if (!(ifs >> parsed.y)) {
            ifs.setstate(std::ios::failbit); // marca falha no stream
            return ifs;
        }

        while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
            ifs.ignore(); // Ignore leading whitespace
        if (ifs.peek() == ',')
            ifs.ignore(); // Ignore ','
        else
            ifs.setstate(std::ios::failbit);

		// Read z
        if (!(ifs >> parsed.z)) {
            ifs.setstate(std::ios::failbit); // marca falha no stream
            return ifs;
        }
        while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
            ifs.ignore(); // Ignore leading whitespace
        if (ifs.peek() == ')')
            ifs.ignore(); // Ignore ')'
        else
            ifs.setstate(std::ios::failbit);

		v = parsed; // Assign parsed values to v
    }
    catch (...) {
        ifs.setstate(std::ios::failbit);
    }
    return ifs;
}

using Vector3 = Vec3<float>;
using Vector3i = Vec3<int>;


template <typename T>
struct Transf2x3 /* Matriz column-major 3x3 representando uma transformação 2D ocultando a última linha de coordenadas homogêneas. */
{
    Vec2<T>columns[3];

    constexpr Transf2x3() = default;
    constexpr Transf2x3(Vec2<T> translation) : columns{ {1, 0}, {0, 1}, translation } {}
    constexpr Transf2x3(Vec2<T> v1_xaxis, Vec2<T> v2_yaxis, Vec2<T> v3_translation = {}) : columns{v1_xaxis, v2_yaxis, v3_translation} {}
    constexpr Transf2x3(T xaxis_x, T xaxis_y, T yaxis_x, T yaxis_y, T translation_x = 0, T translation_y = 0) : columns{
        { xaxis_x, xaxis_y },
        { yaxis_x, yaxis_y },
        { translation_x, translation_y }
    } {}
    constexpr Transf2x3(T translation_x, T translation_y) : Transf2x3{Vec2<T>{ translation_x, translation_y }} {}

    constexpr Transf2x3& operator*=(Transf2x3<T> m) {
        columns[0] = m * columns[0];
        columns[1] = m * columns[1];
        columns[2] = m * columns[2];
        return *this;
    }

    constexpr Transf2x3(Angle theta, Vec2<T> translation = {}) : columns(
        { cosf(theta), sinf(theta) },
        { -sinf(theta), cosf(theta) },
        translation
    ) {}

    constexpr inline Transf2x3& translate(Vec2<T> direction) {
        return *this *= Transf2x3<T>{direction};
    }
    constexpr inline Transf2x3& rotate(float angle) {
        return *this *= Transf2x3{angle};
    }

    template <std::convertible_to<T> U>
    constexpr inline auto transform(Vec2<U> v) const {
        return (*this) * v;
    }
    
    template <std::convertible_to<T> U>
    constexpr inline auto transform(U x, U y) const {
        return (*this) * Vec2<LargerType<T, U>>( x, y );
    }

    constexpr inline const T get(std::size_t col, std::size_t row) const {
        assert_err(col < 3 && row < 3, "Index out of range.");
        if (row == 2)
            return (T)(col == 2); // Homogeneous coordinates.
        return columns[col][row];
    }

    constexpr inline T& get(std::size_t col, std::size_t row) {
        assert_err(col < 3 && row < 2, "Inaccessible index or out of bounds.");
        return columns[col][row];
    }

    constexpr inline T& set(std::size_t col, std::size_t row, T value) {
        assert_err(col < 3 && row < 2, "Inaccessible index or out of bounds.");
        return columns[col][row] = value;
    }
};


// Point transformation.
template<typename T, std::convertible_to<T> U>
inline Vec2<LargerType<T, U>> operator*(const Transf2x3<T>& m, const Vec2<U>& v) {
    return {
        m.columns[0].x * v.x + m.columns[1].x * v.y + m.columns[2].x,
        m.columns[0].y * v.x + m.columns[1].y * v.y + m.columns[2].y
    };
}

// Não definido para respeitar a regra da ordem das matrizes.
//template<typename T, std::convertible_to<T> U>
//inline auto& operator*=(Vec2<U>& v, const Transf2x3<T>& m) {
//    v.x = m.columns[0].x * v.x + m.columns[1].x * v.y + m.columns[2].x;
//    v.y = m.columns[0].y * v.x + m.columns[1].y * v.y + m.columns[2].y;
//    return v;
//}

template <typename T, std::convertible_to<T> U>
inline Transf2x3<LargerType<T, U>> operator*(const Transf2x3<T>& a, const Transf2x3<U>& b) {
    return {
        a * b.columns[0],
        a * b.columns[1],
        a * b.columns[2]
    };
}

template <typename T, std::convertible_to<T> U>
inline Transf2x3<LargerType<T, U>> operator*=(Transf2x3<T>& a, const Transf2x3<U>& b) {
    return a = a * b;
}

using Transform2D = Transf2x3<float>;


struct ColorRgb {
    unsigned char r = 0, g = 0, b = 0;

    constexpr ColorRgb() = default; // implicit constructor
    constexpr ColorRgb(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
    inline Vector3 normalized() const { return Vector3(r / 255.0f, g / 255.0f, b / 255.0f); }
    bool operator==(ColorRgb c) {
        return r == c.r && g == c.g && b == c.b;
    }
    bool operator!=(ColorRgb other) {
        return !(*this == other);
    }
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

    friend std::ostream& operator<<(std::ostream& os, Color c);
    friend std::istream& operator>>(std::istream& is, Color& c);

    friend std::ofstream& operator<<(std::ofstream& ofs, Color c);
    friend std::ifstream& operator>>(std::ifstream& ifs, Color& c);
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

template <typename T>
class Vec2Interpolator {
    const float ZERO_ERROR = 1e-5f; // comparação com zero
public:
    Vec2Interpolator(Vec2<T> from, Vec2<T> to, float step_size)
        : from(from), to(to), step_size(step_size)
    {
        total_distance = from.distance(to);

        if (total_distance < ZERO_ERROR) {
            steps = 1;
        }
        else {
            steps = std::max(2, static_cast<int>(std::ceil(total_distance / step_size)));
        }
    }

    class Iterator {
    public:
        Iterator(Vec2Interpolator* interpolator, int step)
            : interpolator(interpolator), step(step) {
        }

        Vec2<T> operator*() const {
            float t = (interpolator->steps > 1)
                ? static_cast<float>(step) / (interpolator->steps - 1)
                : 0.0f;
            return interpolator->from.lerp(interpolator->to, t);
        }

        Iterator& operator++() {
            step++;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return step != other.step;
        }

    private:
        Vec2Interpolator* interpolator;
        int step;
    };

    Iterator begin() { return Iterator(this, 0); }
    Iterator end() { return Iterator(this, steps); }
    int getSteps() const { return steps; }

private:
    Vec2<T> from;
    Vec2<T> to;
    float step_size;
    float total_distance;
    int steps;
};

template <typename T>
template <std::convertible_to<T> U>
inline Vec2Interpolator<LargerType<T, U>> Vec2<T>::lerpIterator(Vec2<U> to, float by) const {
    return { *this, to, by };
}

} // namespace cg
