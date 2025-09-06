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
#include <fstream>
#include <concepts>


namespace cg
{

template<typename T, typename U>
using LargerType = typename std::common_type<T, U>::type;

// TODO -> default tau/pi precision to float
template <typename T>
constexpr T TAU = std::numbers::pi_v<T> * static_cast<T>(2);

template <typename T>
constexpr T PI = std::numbers::pi_v<T>;

constexpr float EPSILON_ERROR = 1e-6f; // approx. zero
inline constexpr float ZERO_PRECISION_ERROR = EPSILON_ERROR; // alias for EPSILON_ERROR

// Radians to degrees
template <typename T>
inline constexpr T rad_to_deg(T rad) {
    return rad * 180.0 / PI<T>;
}

// Degrees to radians
template <typename T>
inline constexpr T deg_to_rad(T deg) {
    return deg * PI<T> / 180.0;
}

using Angle = float; // Angle in radians.

template <typename T>
class Vec2Interpolator; // forward definition

template <typename T>
struct Vec2 {
    T x = 0, y = 0;

    // Construtores
    constexpr Vec2() = default;
    constexpr Vec2(T x, T y) : x(x), y(y) {}
    constexpr Vec2(T xy) : Vec2{ xy, xy } {}

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

    constexpr operator bool() {
        return *this != Vec2<T>{0, 0};
    }

	constexpr Vec2 operator+() const { return *this; }
	constexpr Vec2 operator-() const { return Vec2<T>{ -x, -y }; }
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

    constexpr inline Vec2<T> abs() const {
        return { std::abs(x), std::abs(y) };
    }

    /* Get linear interpolation next step. */
    inline Vec2 lerp(Vec2 to, float by) const {
        return { x + (to.x - x) * by, y + (to.y - y) * by };
    }

    /* Returns a linear interpolation Iterator towards `to` by the given amount. */
    template <std::convertible_to<T> U>
    Vec2Interpolator<LargerType<T, U>> lerpIterator(Vec2<U> to, float by) const;

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Vec2<U>& v);
    template <typename U>
    friend std::istream& operator>>(std::istream& is, Vec2<U>& v);
    
    template <typename U>
    friend std::ofstream& operator<<(std::ofstream& ofs, const Vec2<U>& v);
    template <typename U>
    friend std::ifstream& operator>>(std::ifstream& ifs, Vec2<U>& v);

    static inline constexpr Vec2<T> zero()  noexcept { return {}; }
    static inline constexpr Vec2<T> one()  noexcept { return { 1, 1 }; }
    static inline constexpr Vec2<T> left()  noexcept { return { -1, 0 }; }
    static inline constexpr Vec2<T> right() noexcept { return { 1, 0 }; }
    static inline constexpr Vec2<T> up()    noexcept { return { 0,  -1 }; }
    static inline constexpr Vec2<T> down()  noexcept { return { 0, 1 }; }
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
    ofs << '(' << v.x << ' ' << v.y << ')'; // Save in the format (x y)
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

        // Load in the format (x, y)
        if (ifs.peek() == '(')
		    ifs.ignore(); // Ignore '('
        else
            ifs.setstate(std::ios::failbit);

        Vec2<T> parsed;
        std::string dummy;
        // Read x y)
        if (!(ifs >> parsed.x) || !(ifs >> parsed.y) || !(ifs >> dummy) || dummy != ")") {
            ifs.setstate(std::ios::failbit); // marca falha no stream
            return ifs;
        }

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

template <typename T, std::convertible_to<T> U>
constexpr bool operator<(Vec2<T> u, Vec2<U> v) { return u.x < v.x || u.y < v.y; }

template <typename T, std::convertible_to<T> U>
constexpr bool operator<=(Vec2<T> u, Vec2<U> v) { return u.x <= v.x || u.y <= v.y; }

template <typename T, std::convertible_to<T> U>
constexpr bool operator>(Vec2<T> u, Vec2<U> v) { return u.x > v.x || u.y > v.y; }

template <typename T, std::convertible_to<T> U>
constexpr bool operator>=(Vec2<T> u, Vec2<U> v) { return u.x >= v.x || u.y >= v.y; }

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

    constexpr Vec3& operator+() const { return *this; }
	constexpr Vec3& operator-() const { return *this = { -x, -y, -z }; }

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

    template<typename U>
    friend std::ostream& operator<<(std::ostream& os, const Vec3<U>& v);
    template<typename U>
    friend std::istream& operator>>(std::istream& is, Vec3<U>& v);

    template<typename U>
    friend std::ofstream& operator<<(std::ofstream& ofs, const Vec3<U>& v);
    template<typename U>
    friend std::ifstream& operator>>(std::ifstream& ifs, Vec3<U>& v);
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
    ofs << "(" << v.x << " " << v.y << " " << v.z << ")"; // Save in the format (x y z)
    return ofs;
}
template <typename T>
inline std::ifstream& operator>>(std::ifstream& ifs, Vec3<T>& v) {
    try {
        while (ifs.peek() == ' ' || ifs.peek() == '\n' || ifs.peek() == '\r')
            ifs.ignore(); // Ignore leading whitespace

        // Load in the format (x y z)
        if (ifs.peek() == '(')
            ifs.ignore(); // Ignore '('
        else
            ifs.setstate(std::ios::failbit);

        Vec3<T> parsed;
		std::string dummy;
		// Read x y z)
        if (!(ifs >> parsed.x) || !(ifs >> parsed.y) || !(ifs >> parsed.z) || !(ifs >> dummy) || dummy != ")") {
            ifs.setstate(std::ios::failbit); // marca falha no stream
            return ifs;
        }

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

    constexpr Transf2x3() : columns{ Vec2<T>(1, 0), Vec2<T>(0, 1), Vec2<T>() } {}
    constexpr ~Transf2x3() = default;
    constexpr Transf2x3(Vec2<T> translation) : columns{ Vec2<T>(1, 0), Vec2<T>(0, 1), translation } {}
    constexpr Transf2x3(Vec2<T> v1_xaxis, Vec2<T> v2_yaxis, Vec2<T> v3_translation = {}) : columns{v1_xaxis, v2_yaxis, v3_translation} {}
    constexpr Transf2x3(T xaxis_x, T xaxis_y, T yaxis_x, T yaxis_y, T translation_x = 0, T translation_y = 0) : columns{
        { xaxis_x, xaxis_y },
        { yaxis_x, yaxis_y },
        { translation_x, translation_y }
    } {}
    constexpr Transf2x3(T translation_x, T translation_y) : Transf2x3{ Vec2<T>{ translation_x, translation_y } } {}

    constexpr Transf2x3(Angle theta, Vec2<T> translation = {}) : columns{
        Vec2<T>{ std::cos(theta), std::sin(theta) },
        Vec2<T>{ -std::sin(theta), std::cos(theta) },
        translation
    } {}

    constexpr inline Transf2x3& translate(Vec2<T> direction) {
        return *this *= Transf2x3<T>{ direction };
    }

    constexpr inline Transf2x3& rotate(float angle) {
        return *this *= Transf2x3<T>{ angle };
    }

    constexpr inline Transf2x3& scale(Vec2<T> by) {
        return *this *= Transf2x3<T>{ Vec2<T>(by.x, 0), Vec2<T>(0, by.y) };
    }

    template <std::convertible_to<T> U>
    constexpr inline auto transform(Vec2<U> v) const {
        return (*this) * v;
    }
    
    template <std::convertible_to<T> U>
    constexpr inline auto transform(U x, U y) const {
        return (*this) * Vec2<LargerType<T, U>>( x, y );
    }

    constexpr inline float determinant() const {
        return columns[0].x * columns[1].y - columns[0].y * columns[1].x;
	}

    constexpr inline auto inverse() const {
        // Inverso de uma matriz de transformação afim 2D
        T det = determinant();
        assert_err(det != 0, "Matrix is not invertible.");
        T invDet = 1 / det;
        Vec2<T> invX{
            columns[1].y * invDet,
            -columns[0].y * invDet
        };
        Vec2<T> invY{
            -columns[1].x * invDet,
            columns[0].x * invDet
        };
        Vec2<T> invT{
            -(invX.x * columns[2].x + invY.x * columns[2].y),
            -(invX.y * columns[2].x + invY.y * columns[2].y)
        };
        return Transf2x3<T>{invX, invY, invT};
	}

	// Faz uma translação absoluta - em relação à origem.
    constexpr inline void translateTo(Vec2<T> position) {
		//*this = Transf2x3<T>{ position } * *this * Transf2x3<T>{-getOrigin()};
        // Equivalente, mas melhor
		*this *= Transf2x3<T>{ position - getOrigin() };
    }

	// Faz uma rotação abosoluta - em relação à origem, considerando que a matriz é uniforme.
    constexpr inline void uniformRotateTo(float angle) {
		assert_err(isUniform(), "Matrix is not uniform.");

        // ângulo atual a partir da coluna 0
        float current = std::atan2(columns[0].y, columns[0].x);
        float delta = angle - current;
        // normalize delta se quiser ficar no intervalo [-pi,pi]
        // delta = fmod(delta + M_PI, 2*M_PI) - M_PI;
        rotate(delta);
    }

    constexpr bool isUniform(float eps = EPSILON_ERROR) const {
        Vector2 u = columns[0]; // (a, b)
        Vector2 v = columns[1]; // (c, d)

        float dot = u.x * v.x + u.y * v.y;
        float lenU = std::sqrt(u.x * u.x + u.y * u.y);
        float lenV = std::sqrt(v.x * v.x + v.y * v.y);

        // shear ≈ 0
        if (std::fabs(dot) > eps * lenU * lenV)
            return false;

        // escalas iguais
        if (std::fabs(lenU - lenV) > eps * std::max(lenU, lenV))
            return false;

        return true;
    }

	// Faz uma rotação absoluta - em relação à origem
	// Prefira uniformeRotateTo se a matriz for uniforme
    constexpr inline void rotateTo(Angle angle) {
        // L = [ a c ]
        // [ b d ]
        float a = columns[0].x;
        float b = columns[0].y;
        float c = columns[1].x;
        float d = columns[1].y;

        // Form S = L^T * L (symmetric 2x2)
        float s11 = a * a + b * b;
        float s12 = a * c + b * d;
        float s22 = c * c + d * d;

        // trace & det
        float tr = s11 + s22;
        float det = s11 * s22 - s12 * s12;
        float discr = tr * tr - 4.0f * det;

        // degenerate guard
        if (discr <= 0.0f) {
            // fallback simples
            float current = std::atan2(b, a);
            rotate(angle - current);
            return;
        }

        float sqrt_discr = std::sqrt(discr);
        float lambda1 = 0.5f * (tr + sqrt_discr);
        float lambda2 = 0.5f * (tr - sqrt_discr);

        // assegura valores positivos (S deve ser p.d.)
        if (lambda1 <= EPSILON_ERROR || lambda2 <= EPSILON_ERROR) {
            float current = std::atan2(b, a);
            rotate(angle - current);
            return;
        }

        // calculamos autovetores de S para montar Q (ortogonal)
        // S = [s11 s12; s12 s22]
        auto make_eigvec = [&](float lambda, float& vx, float& vy) {
            if (std::fabs(s12) > EPSILON_ERROR) {
                vx = s12;
                vy = lambda - s11;
            }
            else {
                // S é quase diagonal
                if (s11 >= s22) { vx = 1.0f; vy = 0.0f; }
                else { vx = 0.0f; vy = 1.0f; }
            }
            // normalize
            float norm = std::sqrt(vx * vx + vy * vy);
            if (norm > EPSILON_ERROR) { vx /= norm; vy /= norm; }
            else { vx = 1.0f; vy = 0.0f; }
            };

        float q1x, q1y, q2x, q2y;
        make_eigvec(lambda1, q1x, q1y);
        // q2 = perpendicular to q1 to form orthonormal basis
        q2x = -q1y; q2y = q1x;

        // inv_sqrt_diag = diag(1/sqrt(lambda1), 1/sqrt(lambda2))
        float i_sqrt1 = 1.0f / std::sqrt(lambda1);
        float i_sqrt2 = 1.0f / std::sqrt(lambda2);

        // invSqrt = Q * diag(...) * Q^T, compute entries explicitly:
        // invSqrt = q1 q1^T * i_sqrt1 + q2 q2^T * i_sqrt2
        float inv00 = q1x * q1x * i_sqrt1 + q2x * q2x * i_sqrt2;
        float inv01 = q1x * q1y * i_sqrt1 + q2x * q2y * i_sqrt2;
        float inv10 = inv01;
        float inv11 = q1y * q1y * i_sqrt1 + q2y * q2y * i_sqrt2;

        // R = L * invSqrt  (2x2 multiplication)
        float R00 = a * inv00 + c * inv10;
        float R01 = a * inv01 + c * inv11;
        float R10 = b * inv00 + d * inv10;
        float R11 = b * inv01 + d * inv11;

        // angle from R (R should be a rotation matrix)
        float current = std::atan2(R10, R00);
        float delta = angle - current;
        rotate(delta);
	}

    // Faz uma escala absoluta em relação à uma matriz não uniforme.
    constexpr inline void scaleTo(Vector2 to) {
        // escala atual
        float sx = columns[0].norm();
        float sy = columns[1].norm();

        // evitar divisão por zero
        if (sx < ZERO_PRECISION_ERROR) sx = 1.0f;
        if (sy < ZERO_PRECISION_ERROR) sy = 1.0f;

        // fatores relativos
        float delta_x = to.x / sx;
        float delta_y = to.y / sy;

        // aplica escala relativa
        *this *= Transf2x3<T>{ delta_x, 0.0f, 0.0f, delta_y };
    }

	// Faz uma escala absoluta uniforme
	// Assume que a matriz é uniforme
    constexpr inline void uniformScaleTo(float to) {
        // escala atual
        float sx = columns[0].norm();

        if (sx < ZERO_PRECISION_ERROR) sx = 1.0f;

        float delta = to / sx;

        // aplica escala relativa uniforme
        *this *= Transf2x3<T>{ delta, 0.0f, 0.0f, delta };
    }

    // Retorna a rotação uniforme
    // Assume que a matriz é uniforme
    constexpr inline Angle getUniformRotation() const {
        return atan2f(columns[0].y, columns[0].x); // or -atan2f(-columns[1].x, columns[1].y);
	}

    // Retorna a rotação da matriz não uniforme.
    // Prefira `getUniformRotation` se a matriz não for uniforme.
    constexpr inline Angle getRotation() const {
        // Decomposição polar simplificada
        float det = columns[0].x * columns[1].y - columns[0].y * columns[1].x;
        float sign = det > 0 ? 1.0f : -1.0f;

        // Extrai rotação ignorando escala
        float norm_x = columns[0].norm();
        float norm_y = columns[1].norm();

        if (norm_x < ZERO_PRECISION_ERROR || norm_y < ZERO_PRECISION_ERROR)
            return 0.0f;

        Vec2<T> rotated_x = columns[0] / norm_x;
        Vec2<T> rotated_y = columns[1] / norm_y;

        // Usa a média dos ângulos dos dois eixos para maior precisão
        float angle1 = atan2f(rotated_x.y, rotated_x.x);
        float angle2 = atan2f(rotated_y.y, rotated_y.x) - sign * PI<float> / 2.0f;

        return (angle1 + angle2) / 2.0f;
    }

    // Obtém a escala não uniforme de cada eixo.
    constexpr inline Vec2<T> getScale() const {
		return { columns[0].norm(), columns[1].norm() }; // usa a norma (length) dos vetores para escala não uniforme
    }

    // Move a origem do sistema de coordenadas local para a posição absoluta (ignorando a rotação).
    // Equivalente a definir a coluna de translação diretamente.
    constexpr inline void setOrigin(Vec2<T> position) {
        columns[2] = position;
    }

	// Retorna a coluna de translação (origem do sistema de coordenadas local) em posição absoluta.
    constexpr inline Vec2<T> getOrigin() const {
        return columns[2];
	}

    constexpr inline T get(std::size_t col, std::size_t row) const {
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

	template<typename U>
    friend std::ostream& operator<<(std::ostream& os, const Transf2x3<U>& m);
    template<typename U>
    friend std::istream& operator>>(std::istream& is, Transf2x3<U>& m);
    template<typename U>
    friend std::ofstream& operator<<(std::ofstream& ofs, const Transf2x3<U>& m);
    template<typename U>
	friend std::ifstream& operator>>(std::ifstream& ifs, Transf2x3<U>& m);
};

template<typename U>
std::ostream& operator<<(std::ostream& os, const Transf2x3<U>& m)
{
	return os << "[x: " << m.columns[0]
             << "; y: " << m.columns[1]
             << "; t: " << m.columns[2] << ']';
}
template<typename U>
std::istream& operator>>(std::istream& is, Transf2x3<U>& m)
{
    try {
        char c;
        // Read in the format [x: (x.x, x.y); y: (y.x, y.y); t: (t.x, t.y)]
        if (!(is >> c >> c >> m.columns[0] >> c >> c >> m.columns[1] >> c >> c >> m.columns[2] >> c))
            is.setstate(std::ios::failbit); // marca falha no stream
    }
    catch (...) {
        is.setstate(std::ios::failbit);
	}
    return is;
}

template<typename U>
std::ofstream& operator<<(std::ofstream& ofs, const Transf2x3<U>& m)
{
    ofs << "[x: " << m.columns[0] << "; y: " << m.columns[1] << "; t: " << m.columns[2] << ']';
    return ofs;
}

template<typename U>
std::ifstream& operator>>(std::ifstream& ifs, Transf2x3<U>& m)
{
    try {
        std::string dummy;
        Vector2 parsed[3];
        // Read in the format [x: (x.x, x.y); y: (y.x, y.y); t: (t.x, t.y)]
        if (!(ifs >> dummy >> parsed[0]) || dummy != "[x:" ||
            !(ifs >> parsed[1] >> dummy) || dummy != ";y:" ||
            !(ifs >> parsed[2] >> dummy) || dummy != ";t:")
        {
            ifs.setstate(std::ios::failbit); // marca falha no stream
            return ifs;
        }
    }
    catch (...) {
        ifs.setstate(std::ios::failbit);
    }
    return ifs;
}

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
    // transformação considerando coordenadas homogêneas [h = (0, 0)]
    auto transform_axis_vector = [&](Vec2<T> v) {
        return Vec2<LargerType<T, U>>{
            a.columns[0].x * v.x + a.columns[1].x * v.y,
            a.columns[0].y * v.x + a.columns[1].y * v.y };
    };

    return {
        transform_axis_vector(b.columns[0]),
        transform_axis_vector(b.columns[1]),
        a * b.columns[2] // transformação completa no vetor de translação [h = 1]
    };
}


template <typename T, std::convertible_to<T> U>
inline Transf2x3<LargerType<T, U>>& operator*=(Transf2x3<T>& a, const Transf2x3<U>& b) {
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
    constexpr Color BLACK   { 0.0f, 0.0f, 0.0f };
    constexpr Color WHITE   { 1.0f, 1.0f, 1.0f };
    constexpr Color RED     { 1.0f, 0.0f, 0.0f };
    constexpr Color GREEN   { 0.0f, 1.0f, 0.0f };
    constexpr Color BLUE    { 0.0f, 0.0f, 1.0f };
    constexpr Color MAGENTA { 1.0f, 0.0f, 1.0f };
    constexpr Color CYAN  { 0.0f, 1.0f, 1.0f };
    constexpr Color YELLOW  { 1.0f, 1.0f, 0.0f };
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

// deltas

using DeltaTime = float; // △t s
using DeltaAngle = Angle; // △θ rad
using Vec2Offset = Vector2; // (△x, △y)
using Velocity = Vec2Offset; // △v = (△x, △y)

} // namespace cg
