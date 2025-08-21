#pragma once

#include <vector>

#include <cg/canvas.hpp>
#include <cg/geometry.hpp>
#include <cg/geometry/triangulation.hpp>

#include "cg/canvas_itens/line.hpp"

namespace cg
{
    class Polygon : public CanvasItem
    {
        public:
            Polygon() = default;
            Polygon(Vector2 position, Color color = Color{}) : CanvasItem{ position }, innerColor{ color } {}


            void _render() override;

            inline void append(Vector2 newVertex) {
                vertices.push_back(newVertex);
            }

            // Polygon size ignoring the vertice on its start position.
            inline size_t size() {
                return vertices.size();
            }

            inline Vector2& lastVertice() {
                return (vertices.size() == 0) ? position : vertices.back();
            }

            inline Vector2 lastVertice() const {
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

            inline Color& getColor() {
                return innerColor;
            }

            inline Color getColor() const {
                return innerColor;
            }
        private:
            std::vector<Vector2> vertices;
            Color innerColor;
            //Color contourColor; // TODO
            Triangulation triangulation;
    };
} // namespace cg
