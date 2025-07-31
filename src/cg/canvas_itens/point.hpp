#pragma once

#include "../canvas.hpp"

#include <cg/geometry.hpp>

namespace cg
{
    class Point : public CanvasItem
    {
    public:
        Point() = default;

        int init(ColorRgb pointColor, ArrayList<int> coords);

        void _process(DeltaTime delta) override;

        void _render() override;

        void _input(io::MouseMove input_event) override;
    };
    
} // namespace cg
