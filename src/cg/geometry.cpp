#include "../api.hpp"

#include <cmath>

#include "geometry.hpp"


namespace cg
{

void genSemiArcOverCircle(const Vector2& arcCenter, float innerRadius,
        float outerRadius, const Vector2& circleCenter, float circleRadius,
        std::size_t segments, std::size_t edgeSegments)
{
    // 1) distância e direção entre centros
    float dx = circleCenter.x - arcCenter.x;
    float dy = circleCenter.y - arcCenter.y;
    float d  = std::sqrt(dx*dx + dy*dy);
    if (d < 1e-6f) return;
    float phi = std::atan2(dy, dx);

    // 2) intervalo de ângulos no arco interno/externo (lei dos cossenos)
    auto computeInterval = [&](float R, float& t0, float& t1){
        float cosA = (d * d + R * R - circleRadius * circleRadius) / (2.0f * d * R);
        cosA = std::clamp(cosA, -1.0f, 1.0f);
        float alpha = std::acos(cosA);
        t0 = phi - alpha;
        t1 = phi + alpha;
    };
    float in0, in1, out0, out1;
    computeInterval(innerRadius, in0, in1);
    computeInterval(outerRadius, out0, out1);

    // 3) ângulos de interseção NO PERÍMETRO DO CÍRCULO AZUL,
    //    usando o raio correto para cada t
    auto intersectAng = [&](float R, float theta){
        Vector2 p{ arcCenter.x + R * std::cos(theta),
                arcCenter.y + R * std::sin(theta) };
        return std::atan2(p.y - circleCenter.y,
                          p.x - circleCenter.x);
    };
    float angIn0  = intersectAng(innerRadius, in0);
    float angOut0 = intersectAng(outerRadius, out0);
    float angIn1  = intersectAng(innerRadius, in1);
    float angOut1 = intersectAng(outerRadius, out1);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Modo wireframe

    // 4) monta o TRIANGLE_STRIP
    glBegin(GL_TRIANGLE_STRIP);

    // 4.1) lateral esquerda: interpolar de angOut0 → angIn0
    for (std::size_t i = 0; i <= edgeSegments; ++i) {
        float t    = float(i) / edgeSegments;
        float aOut = angOut0*(1 - t) + angIn0*t;
        float aIn  = angOut0*(1 - t) + angIn0*t;
        // ponto na borda externa (círculo azul)
        glVertex2f(
            circleCenter.x + circleRadius * std::cos(aOut),
            circleCenter.y + circleRadius * std::sin(aOut)
        );
        // ponto na borda interna (também no círculo azul)
        glVertex2f(
            circleCenter.x + circleRadius * std::cos(aIn),
            circleCenter.y + circleRadius * std::sin(aIn)
        );
    }

    // 4.2) arcos principal (outer → inner) no centro da faixa
    for (std::size_t i = 0; i <= segments; ++i) {
        float to = out0 + (out1 - out0) * (float(i) / segments);
        float ti = in0  + (in1 - in0) * (float(i) / segments);
        // externo
        glVertex2f(
            arcCenter.x + outerRadius * std::cos(to),
            arcCenter.y + outerRadius * std::sin(to)
        );
        // interno
        glVertex2f(
            arcCenter.x + innerRadius * std::cos(ti),
            arcCenter.y + innerRadius * std::sin(ti)
        );
    }

    // 4.3) lateral direita: interpolar de angIn1 → angOut1
    for (std::size_t i = 0; i <= edgeSegments; ++i) {
        float t    = float(i) / edgeSegments;
        float aOut = angIn1 * (1 - t) + angOut1*t;
        float aIn  = angIn1 * (1 - t) + angOut1*t;
        glVertex2f(
            circleCenter.x + circleRadius * std::cos(aOut),
            circleCenter.y + circleRadius * std::sin(aOut)
        );
        glVertex2f(
            circleCenter.x + circleRadius * std::cos(aIn),
            circleCenter.y + circleRadius * std::sin(aIn)
        );
    }

    glEnd();
}


/* Função para calcular o ângulo médio do arco visível */
std::pair<float, float> computeArcAngles(const Vector2& arcCenter, float radius, const Vector2& circleCenter, float circleRadius)
{
    // 1. Calcular vetor entre centros
    float dx = circleCenter.x - arcCenter.x;
    float dy = circleCenter.y - arcCenter.y;
    float d = std::sqrt(dx*dx + dy*dy);

    // 2. Calcular ângulo de direção entre centros
    float phi = std::atan2(dy, dx);

    // 3. Calcular ângulo de abertura usando lei dos cossenos
    float cosAlpha = (d * d + radius * radius - circleRadius * circleRadius) / (2.0f * d * radius);
    cosAlpha = std::clamp(cosAlpha, -1.0f, 1.0f);
    float alpha = std::acos(cosAlpha);

    return {phi - alpha, phi + alpha};
}


void drawArcText(const char* text, float radius, float startAngle, float endAngle, float scale)
{
    if (!text || *text == '\0') return;

    // Calcular ângulo total e direção
    float arcAngle = endAngle - startAngle;
    bool clockwise = (arcAngle < 0);
    float absArcAngle = std::abs(arcAngle);

    // Pré-calcular largura total do texto (unscaled)
    float totalWidth = 0;
    for (const char* c = text; *c; c++) {
        totalWidth += glutStrokeWidth(GLUT_STROKE_ROMAN, *c);
    }
    if (totalWidth <= 0) return;

    float cumulativeWidth = 0;

    for (const char* c = text; *c; c++) {
        float charWidth = glutStrokeWidth(GLUT_STROKE_ROMAN, *c);
        // Cálculo do ângulo para este caractere
        float charAngle;
        float t = (cumulativeWidth + charWidth / 2) / totalWidth;

        if (clockwise) {
            charAngle = startAngle - t * absArcAngle;
        } else {
            charAngle = startAngle + t * absArcAngle;
        }

        glPushMatrix();
        // Posicionar no ponto do arco
        float x = radius * cos(charAngle);
        float y = radius * sin(charAngle);
        glTranslatef(x, y, 0);

        // Orientação para caracteres "em pé"
        float rotation = (charAngle + PI<float> / 2) * 180.0f / PI<float>;

        // Ajuste para orientação correta
        if (clockwise) rotation += 180.0f;

        glRotatef(rotation, 0, 0, 1);

        // Centralização
        glTranslatef(-0.38f * charWidth * scale, 0.05f * charWidth * scale, 0);
        glScalef(scale, scale, scale);

        // Renderizar caractere
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
        glPopMatrix();

        cumulativeWidth += charWidth;
    }
}


} // namespace cg
