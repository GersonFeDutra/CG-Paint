#pragma once

#include <vector>

#include <cg/canvas.hpp>

#include <cg/math.hpp>
#include <cg/geometry.hpp>


namespace cg
{
    class Line : public CanvasItem
    {
    public:
        Line() : CanvasItem(TypeInfo::LINE) {}
        Line(Vector2 position, Color color = Color{}) : CanvasItem{ TypeInfo::LINE, position }, color{ color } {}

        //void _process(DeltaTime delta) override;

        void _render() override;

        inline void append(Vector2 vertice) {
            vertices.push_back(vertice);
        }

        // Tamanho da corda, desconsiderando vértice de origem.
        inline size_t size() const {
            return vertices.size();
        }

        inline Vector2& lastVertice() {
            return (size() == 0) ? position : vertices.back();
        }

        inline Vector2 lastVertice() const {
            return (size() == 0) ? position : vertices.back();
        }

        // Guido: adicionei essa função pra ver se um ngc funciona no polygon_tool
        inline std::vector<Vector2> getVertices() {
            return vertices;
        }

        inline Color& getColor() {
            return color;
        }

        inline Color getColor() const {
            return color;
        }

        
        inline void setColor(ColorRgb lineColor) {
            color = lineColor;
        }

        inline void setVertices(std::vector<Vector2> lineVertices) {
            vertices = lineVertices;
        }

        // Inherited via CanvasItem
        std::ostream& _print(std::ostream& os) const override;
        std::ofstream& _serialize(std::ofstream& ofs) const override;
        std::ifstream& _deserialize(std::ifstream& ifs) override;
    private:
        std::vector<Vector2> vertices;
        Color color; // TODO -> alpha blending
		float width = 1.0f; // TODO -> anti-alias
    };

} // namespace cg
