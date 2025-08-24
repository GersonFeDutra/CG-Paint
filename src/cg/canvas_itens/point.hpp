// #pragma once

// #include "../canvas.hpp"

// #include <cg/math.hpp>
// #include <cg/geometry.hpp>


// namespace cg
// {
//     class Point : public CanvasItem
//     {
//     public:
//         inline static const float SIZE = 10.0f;
//     public:
//         Point() : CanvasItem(TypeInfo::POINT) {}
//         Point(Vector2 position, Color color = Color{});

//         //void _process(DeltaTime delta) override;

//         bool _isSelected(Vector2 cursor_local_position) const override;

//         void _render() override;
//         void _input(io::MouseDrag mouse_event) override;

//         // inline Color& getColor() {
//         //     return color;
//         // }
        
//         void setColor(const ColorRgb& newColor) override {
//             color = newColor;
//         }

//         ColorRgb getColor() override {
//             return color;
//         }

//         inline Vector2 getPosition() const {
//             return model * Vector2{};
//         }

//         inline void setPosition(Vector2 to) {
//             model = Transform2D(to);
// 		}

//         //void _input(io::MouseMove input_event) override;

//         // Inherited via CanvasItem
//         std::ostream& _print(std::ostream& os) const override;
//         std::ofstream& _serialize(std::ofstream& ofs) const override;
//         std::ifstream& _deserialize(std::ifstream& ifs) override;
//     private:
//         ColorRgb color{}; // TODO -> alpha blending
// 		float size = SIZE; // TODO -> smooth point
//     };

// } // namespace cg
#pragma once

#include "../canvas.hpp"

#include <cg/math.hpp>
#include <cg/geometry.hpp>


namespace cg
{
    class Point : public CanvasItem
    {
    public:
        inline static const float SIZE = 10.0f;
    public:
        Point() : CanvasItem(TypeInfo::POINT) {}
        Point(Vector2 position, Color color = Color{});

        //void _process(DeltaTime delta) override;

        bool isSelected(Vector2 mousePos) const override;

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

        // Inherited via CanvasItem
        std::ostream& _print(std::ostream& os) const override;
        std::ofstream& _serialize(std::ofstream& ofs) const override;
        std::ifstream& _deserialize(std::ifstream& ifs) override;
    private:
        Color color; // TODO -> alpha blending
		unsigned size = SIZE; // TODO -> smooth point
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