#pragma once

#include "../canvas.hpp"

#include <cg/math.hpp>
#include <cg/geometry.hpp>


namespace cg {

    class Flag : public CanvasItem  {
        static struct ColorSet {
            const ColorRgb GREEN{ 0, 148, 64, };
            const ColorRgb YELLOW{ 255, 203, 0, };
            const ColorRgb BLUE{ 48, 38, 129, };
            const ColorRgb WHITE{ 255, 255, 255, };
        } colors;

    public:
        static constexpr Vector2 SIZE{ 20, 14 };

        Flag();

        void _process(DeltaTime delta) override;

        void _render() override;

        void _input(io::MouseMove input_event) override;

        // Inherited via CanvasItem
        //std::ostream& _print(std::ostream& os) const override;
        std::ostream& _serialize(std::ostream& os) const override;
        std::istream& _deserialize(std::istream& is) override;

        // Inherited via CanvasItem
        bool _isSelected(Vector2 cursor_local_position) const override;
    };
}
