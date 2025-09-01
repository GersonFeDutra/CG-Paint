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
        
        inline Vector2 getPosition() const {
            return model * localPosition;
        }
        
        inline void setPosition(Vector2 to) {
            localPosition = model.inverse() * to;
		}
        
        //void _input(io::MouseMove input_event) override;

    protected:
        bool _isSelected(Vector2 cursor_local_position) const override;

        // Inherited via CanvasItem
        std::ostream& _print(std::ostream& os) const override;
        std::ofstream& _serialize(std::ofstream& ofs) const override;
        std::ifstream& _deserialize(std::ifstream& ifs) override;
                
    private:
        Vector2 localPosition{};
        Color color{}; // TODO -> alpha blending
		float size = SIZE; // TODO -> smooth point
    };

} // namespace cg
