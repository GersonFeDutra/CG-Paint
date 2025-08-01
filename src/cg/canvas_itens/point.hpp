#pragma once

#include "../canvas.hpp"

#include <cg/geometry.hpp>


namespace cg
{
    class Point : public CanvasItem
    {
    public:
        Point() = default;
        Point(Vector2 position, ColorRgb color = ColorRgb{});

        //void _process(DeltaTime delta) override;

        void _render() override;
        void _input(io::MouseDrag mouse_event) override;

        //void _input(io::MouseMove input_event) override;
    private:
        ColorRgb color; // TODO -> alpha blending
        //unsigned size; // TODO -> smooth point
    };

    class Points : public CanvasItem
    {
    public:
        Points() = default;
        Points(ColorRgb point_color, ArrayList<Vector2> point_list);

        //void _process(DeltaTime delta) override;

        void _render() override;

        //void _input(io::MouseMove input_event) override;
    private:
        ColorRgb pointColor;
        ArrayList<Vector2> pointList;
    };

} // namespace cg
