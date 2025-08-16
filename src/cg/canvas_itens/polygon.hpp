#pragma once

#include <vector>

#include <cg/canvas.hpp>
#include <cg/geometry.hpp>

#include "cg/canvas_itens/line.hpp"

namespace cg
{
    class Polygon : public CanvasItem
    {
        public:
            Polygon() = default;
            Polygon(Vector2 position, ColorRgb color = ColorRgb{}) : CanvasItem{ position }, insideColor{ color } {}


            void _render() override;

            inline void append(Vector2 newVertex) {
                vertices.push_back(newVertex);
            }

            // adicionar mais funções aqui embaixo se precisar
            inline Vector2 lastVertice() {
                return (vertices.size() == 0) ? position : vertices.back();
            }

            inline std::vector<Vector2> getVertices() {
                return vertices;
            }

            inline void setVertices(std::vector<Vector2> allVertices) {
                for (Vector2 vertex : allVertices)
                {
                    // std::cout << "Appended vertex " << (vertex.x, vertex.y) << std::endl;
                    append(vertex);
                }
            }

            inline ColorRgb getColor() {
                return insideColor;
            }
        private:
            std::vector<Vector2> vertices;
            ColorRgb insideColor;
    };
} // namespace cg
