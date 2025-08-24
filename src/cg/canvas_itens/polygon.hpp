// #pragma once

// #include <vector>

// #include <cg/canvas.hpp>
// #include <cg/math.hpp>

// #include "../canvas_item.hpp"


// namespace cg
// {
//     class Polygon : public CanvasItem
//     {
//     public:
//         Polygon() : CanvasItem(TypeInfo::POLYGON) {}
//         Polygon(Vector2 position, Color color = Color{}) : CanvasItem{ TypeInfo::POLYGON }, innerColor{ color }, countourColor{ color } {
// 			vertices.reserve(3); // reserva espaço para 3 vértices
//             vertices.emplace_back(position);
//         }
//         Polygon(std::vector<Vector2> verts) : CanvasItem{ TypeInfo::POLYGON }, vertices{ verts } {}

//         void _render() override;

//         // Verifica se o polígono foi selecionado pelo mouse
//         bool _isSelected(Vector2 mousePos) const override;

//         inline void append(Vector2 newVertex) {
//             vertices.push_back(newVertex);
//         }

// 		// Number of vertices
//         inline size_t size() {
//             return vertices.size();
//         }

//         inline Vector2& firstVertice() {
//             assert_err(!vertices.empty(), "No vertices in polygon.");
//             return vertices.front();
//         }

//         inline Vector2& lastVertice() {
// 			assert_err(!vertices.empty(), "No vertices in polygon.");
//             return vertices.back();
//         }

//         inline Vector2 lastVertice() const {
//             assert_err(!vertices.empty(), "No vertices in polygon.");
//             return vertices.back();
//         }

// 		// Returns a copy of the vertices
//         inline std::vector<Vector2> getVertices() {
//             return vertices;
//         }
            
//         inline void setVertices(std::vector<Vector2> allVertices) {
//             vertices = allVertices;
//         }

//         // Métodos virtuais sobrescritos
//         void setColor(const ColorRgb& color) override {
//             innerColor = color;
//         }

//         ColorRgb getColor() override {
//             return innerColor;
//         }

//         // Inherited via CanvasItem
//         std::ostream& _print(std::ostream& os) const override;
//         std::ofstream& _serialize(std::ofstream& ofs) const override;
//         std::ifstream& _deserialize(std::ifstream& ifs) override;
//     private:
//         std::vector<Vector2> vertices;
//         ColorRgb innerColor{};
//         ColorRgb countourColor{}; // TODO -> Implement contour color
// 		float width = 1.0f; // TODO -> Implement line width
//         // TODO -> anti-alias

//     };

// } // namespace cg
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
        Polygon(Vector2 position, Color color = Color{}) : CanvasItem{ TypeInfo::POLYGON, position }, innerColor{ color }, countourColor{ color } {}
        Polygon(std::vector<Vector2> verts) : CanvasItem{ TypeInfo::POLYGON } {
            if (verts.empty())
				return;
			position = verts.front();

			vertices.reserve(verts.size() - 1);
            for (auto& vert : verts)
				vertices.push_back(vert); // Store relative to position
        }

        // Verifica se o pol�gono foi selecionado pelo mouse
        bool isSelected(Vector2 mousePos) const override;

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

        // inline ColorRgb getColor() {
        //     return innerColor;
        // }

            
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
        Color innerColor;
		Color countourColor; // TODO -> Implement contour color
		float width = 1.0f; // TODO -> Implement line width
        // TODO -> anti-alias

    };

} // namespace cg