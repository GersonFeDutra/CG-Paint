#pragma once

#include "../canvas.hpp"

#include <cg/geometry.hpp>


namespace cg
{
    class Point : public CanvasItem
    {
    public:
        inline static const float SIZE = 10.0f;
    public:
        Point() = default;
        Point(Vector2 position, Color color = Color{});

        //void _process(DeltaTime delta) override;

        void _render() override;
        void _input(io::MouseDrag mouse_event) override;

        inline Color& getColor() {
            return color;
        }

        inline Color getColor() const {
            return color;
        }

        inline void setColor(Color newColor) {
            color = newColor;
        }

        //void _input(io::MouseMove input_event) override;
    private:
        Color color; // TODO -> alpha blending
        //unsigned size; // TODO -> smooth point
    };

    class Points : public CanvasItem
    {
    public:
        Points() = default;
        Points(Color point_color, ArrayList<Vector2> point_list);

        //void _process(DeltaTime delta) override;

        void _render() override;

        //void _input(io::MouseMove input_event) override;
    private:
        Color pointColor;
        ArrayList<Vector2> pointList;
    };

} // namespace cg
