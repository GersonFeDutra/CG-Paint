#pragma once

#include <vector>

#include <cg/canvas.hpp>

#include <cg/geometry.hpp>

namespace cg
{
    class Line : public CanvasItem
    {
    public:
        Line() = default;
        Line(Vector2 position, ColorRgb color = ColorRgb{}) : CanvasItem{ position }, color{ color } {}

        //void _process(DeltaTime delta) override;

        void _render() override;

        inline void append(Vector2 vertice) {
            vertices.push_back(vertice);
        }

        // Tamanho da corda, desconsiderando v�rtice de origem.
        inline size_t size() {
            return vertices.size();
        }

        inline Vector2 lastVertice() {
            return (size() == 0) ? position : vertices.back();
        }

        // Guido: adicionei essa função pra ver se um ngc funciona no polygon_tool
        inline std::vector<Vector2> getVertices() {
            return vertices;
        }

        // Guido: sla to com sono
        inline ColorRgb getColor() {
            return color;
        }

        
        inline void setColor(ColorRgb lineColor) {
            color = lineColor;
        }

        inline void setVertices(std::vector<Vector2> lineVertices) {
            vertices = lineVertices;
        }
    private:
        std::vector<Vector2> vertices;
        ColorRgb color; // TODO -> alpha blending
        //unsigned size; // TODO -> smooth point
    };

} // namespace cg
