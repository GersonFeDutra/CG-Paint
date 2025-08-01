#pragma once

#include "../canvas.hpp"

#include <cg/geometry.hpp>

namespace cg
{
    class Points : public CanvasItem
    {
    public:
        Points() = default;
        Points(ColorRgb point_color, ArrayList<int> coords);

        //void _process(DeltaTime delta) override;

        void _render() override;

        //void _input(io::MouseMove input_event) override;
    private:
        ColorRgb pointColor;
        ArrayList<int> coords;
    };
    
} // namespace cg
