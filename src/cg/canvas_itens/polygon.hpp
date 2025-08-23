#pragma once

#include <vector>

#include <cg/canvas.hpp>
#include <cg/math.hpp>

#include "../canvas_item.hpp"


namespace cg
{
    class Polygon : public CanvasItem
    {
    public:
        Polygon() : CanvasItem(TypeInfo::POLYGON) {}
        Polygon(Vector2 position, Color color = Color{}) : CanvasItem{ TypeInfo::POLYGON }, innerColor{ color }, countourColor{ color } {
			vertices.reserve(3); // reserva espaço para 3 vértices
            vertices.emplace_back(position);
        }
        Polygon(std::vector<Vector2> verts) : CanvasItem{ TypeInfo::POLYGON }, vertices{ verts } {}

        void _render() override;

        // Verifica se o polígono foi selecionado pelo mouse
        bool _isSelected(Vector2 mousePos) const override;

        inline void append(Vector2 newVertex) {
            vertices.push_back(newVertex);
        }

		// Number of vertices
        inline size_t size() {
            return vertices.size();
        }

        inline Vector2& lastVertice() {
			assert_err(!vertices.empty(), "No vertices in polygon.");
            return vertices.back();
        }

        inline Vector2 lastVertice() const {
            assert_err(!vertices.empty(), "No vertices in polygon.");
            return vertices.back();
        }

		// Returns a copy of the vertices
        inline std::vector<Vector2> getVertices() {
            return vertices;
        }
            
        inline void setVertices(std::vector<Vector2> allVertices) {
            vertices = allVertices;
        }

        inline void setColor(ColorRgb color) {
            innerColor = color;
        }

        inline Color& getColor() {
            return innerColor;
        }

        inline Color getColor() const {
            return innerColor;
        }

        // Inherited via CanvasItem
        std::ostream& _print(std::ostream& os) const override;
        std::ofstream& _serialize(std::ofstream& ofs) const override;
        std::ifstream& _deserialize(std::ifstream& ifs) override;
    private:
        std::vector<Vector2> vertices;
        Color innerColor{};
        Color countourColor{}; // TODO -> Implement contour color
		float width = 1.0f; // TODO -> Implement line width
        // TODO -> anti-alias

    };

} // namespace cg
