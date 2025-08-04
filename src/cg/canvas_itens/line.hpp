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

        inline size_t size() {
            return vertices.size();
        }

        inline Vector2 frontVertice() {
            assert_err(!vertices.empty(), "Cannot access single vertice line.");
            return vertices.front();
        }
    private:
        std::vector<Vector2> vertices;
        ColorRgb color; // TODO -> alpha blending
        //unsigned size; // TODO -> smooth point
    };

} // namespace cg
