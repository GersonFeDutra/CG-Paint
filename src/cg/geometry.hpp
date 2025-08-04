#pragma once
#include <cg/math.hpp>

#include <algorithm> // std::clamp


namespace cg
{

/**
 * Gera um "anel" parcial (faixa) entre dois raios (innerRadius, outerRadius)
 * de um centro arcCenter, mas somente o trecho que fica DENTRO do círculo
 * (circleCenter, circleRadius).
 */
void genSemiArcOverCircle(const Vector2& arcCenter, float innerRadius,
        float outerRadius, const Vector2& circleCenter, float circleRadius,
        std::size_t segments = 64, std::size_t edgeSegments = 4);


/* Função para calcular o ângulo médio do arco visível */
std::pair<float, float> computeArcAngles(const Vector2& arcCenter, float radius, const Vector2& circleCenter, float circleRadius);


/** Gera um círculo no frame buffer do Open GL
 * @param center Posição do círculo, em relação ao centro
 * @param radius Raio do círculo
 * @param segments "Resolução"/ quantidade de segmentos do polígono gerado
 */
inline void genCircle(Vector2 center, float radius, std::size_t segments)
{
    glBegin(GL_POLYGON); // Inicializa o desenho de um polígono
    float x, y;
    float offset = TAU<float> / segments;
    float angle = 0.0f; // current angle in radians
    for (std::size_t i = 0; i < segments; i++) {
        // define a coordenada de cada ponto no perímetro do círculo
        x = center.x + radius * cosf(angle);
        y = center.y + radius * sinf(angle);

        glVertex2f(x, y);
        angle += offset;
    }
    glEnd();
}


/** Gera um semi-círculo em Open GL frame buffer
 * @param segmentsByArc quantidade de segmentos em cada arco (interno e externo)
 */
inline void genSemiArc(Vector2 center, float innerRadius, float outerRadius,
        float startAngle, float endAngle, std::size_t segmentsByArc) {
    glBegin(GL_LINE_LOOP); // Inicializa o desenho de um polígono
    float x, y;
    float offset = (endAngle - startAngle) / segmentsByArc;
    float angle = startAngle; // current angle in radians

    // Arco externo
    for (std::size_t i = 0; i <= segmentsByArc; i++) {
        // define a coordenada de cada ponto no perímetro do semi-arco
        x = center.x + outerRadius * cosf(angle);
        y = center.y + outerRadius * sinf(angle);

        glVertex2f(x, y);
        angle += offset;
    }

    glColor3ub(0, 0, 0);
    // Arco interno
    for (std::size_t i = 0; i <= segmentsByArc; i++) {
        // define a coordenada de cada ponto no perímetro do semi-arco
        x = center.x + innerRadius * cosf(angle);
        y = center.y + innerRadius * sinf(angle);

        glVertex2f(x, y);
        angle -= offset;
    }
    glEnd();
}


/**
 * Desenha uma estrela regular de 5 pontas.
 * @param center Centro da estrela
 * @param outerRadius Raio externo (pontas)
 * @param innerRatio Razão do raio interno (entre 0 e 1)
 */
inline void genStar(const Vector2& center, float outerRadius, float innerRatio) {
    constexpr int numPoints = 5;
    float innerRadius = outerRadius * std::clamp(innerRatio, 0.0f, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
        // Centro do fan
        glVertex2f(center.x, center.y);

        // Gera os vértices alternando externo e interno
        for (int i = 0; i <= numPoints * 2; ++i) {
            float angle = PI<float> / 2 - i * (PI<float> / numPoints); // começa pra cima
            float radius = (i % 2 == 0) ? outerRadius : innerRadius;

            float x = center.x + radius * std::cos(angle);
            float y = center.y + radius * std::sin(angle);

            glVertex2f(x, y);
        }
    glEnd();
}


/**
 * Desenha um texto ao longo de um arco de círculo.
 *
 * @param text Texto a ser desenhado
 * @param radius Raio do arco
 * @param startAngle Ângulo de início do arco (em radianos)
 * @param endAngle Ângulo de fim do arco (em radianos)
 * @param scale Fator de escala para o texto
 *
 * @note A direção do texto depende do sinal do ângulo de abertura.
 *       Se o ângulo for negativo, o texto será desenhado no sentido horário.
 */
void drawArcText(const char* text, float radius, float startAngle, float endAngle, float scale);


/** Gera um círculo com base na qualidade.
 * @param quality Fator de suavidade ajustável. Quanto maior, mais segmentos.
 * @param min_offset Quantidade mínima de segmentos.
 * @param limit Quantidade máxima de segmentos.
 */
inline void genCircleAuto(Vector2 center, float radius, float quality = 1.5f, int min_offset = 12, int limit = 2048) {
    int segs = int(quality * radius) + min_offset; // Valor base linear + offset mínimo
    genCircle(center, radius, std::min(segs, limit));
}


inline void genCircleAuto(float x, float y, float radius, float quality = 1.5f, int min_offset = 12, int limit = 2048) {
    genCircleAuto({x, y}, radius, quality, min_offset, limit);
}


} // namespace cg
