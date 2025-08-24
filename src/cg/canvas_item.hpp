#pragma once

#include "math.hpp"
#include "input_event.hpp"

namespace cg {
    using ID = std::size_t;
    using DeltaTime = float;

    class CanvasItem {
        friend class Canvas;
        friend class SelectTool;
    public:
        enum class TypeInfo {
            POINT = 0,
            LINE,
            POLYGON,
            OTHER,
        };
    public:
        CanvasItem() = default;
        CanvasItem(TypeInfo type_info) : typeInfo{ type_info } {}
        virtual ~CanvasItem() = default;
        CanvasItem(TypeInfo type_info, Vector2 position) : typeInfo{ type_info }, model(position) {}

        // Make rotate virtual but not pure. Add a default implementation.
        // This avoids errors in classes that don't need a specific rotation logic.
        virtual void rotate(float angle) {
            model.rotate(angle);
        }

        /* Process mouse input from user. [Move] */
        virtual void _input(io::MouseMove mouse_event) {}
        /* Process mouse drag input from user. */
        virtual void _input(io::MouseDrag mouse_event) {}
        /* Called when right button is pressed. */
        virtual void _input(io::MouseRightButtonPressed mouse_event) {}
        /* Called when left button is pressed. */
        virtual void _input(io::MouseLeftButtonPressed mouse_event) {}
        /* Called when right button is released. */
        virtual void _input(io::MouseRightButtonReleased mouse_event) {}
        /* Called when left button is released. */
        virtual void _input(io::MouseLeftButtonReleased mouse_event) {}
        /* Process mouse wheel vertical input from user. */
        virtual void _input(io::MouseWheelV mouse_event) {}
        /* Process mouse wheel horizontal input from user. */
        virtual void _input(io::MouseWheelH mouse_event) {}
        virtual void _input(io::KeyInputEvent key_event) {}
        
        /** Process data between variations of delta time △t.
        * @param delta : time stamp between previous frame
        */
        virtual void _process(DeltaTime delta) {}
        /* Draw data onto screen with open GL calls. */
        virtual void _render() {}
        
        // Métodos para cores virtuais. Revertidos para ColorRgb para consistência.
        virtual void setColor(const ColorRgb& color) = 0;
        virtual ColorRgb& getColor() const = 0;

        inline bool isSelected(Vector2 mouse_position) const { return _isSelected(toLocal(mouse_position)); }

        virtual bool _isSelected(Vector2 cursor_local_position) const = 0;

        inline Vector2 toLocal(Vector2 global_position) const { return model.inverse() * global_position; }
        
        inline void translate(Vector2 by) {
            model.translate(by);
        }

        inline void translateTo(Vector2 to) {
            model.translateTo(to);
        }
        
        inline TypeInfo getTypeInfo() const {
            return typeInfo;
        }

        friend std::ostream& operator<<(std::ostream& os, const CanvasItem& item);
        friend std::istream& operator>>(std::istream& is, CanvasItem& item);
        virtual std::ostream& _print(std::ostream& os) const = 0;

        // No need for a separate read method, the operator overload can handle it.
        // It's also not declared as pure virtual, so it would need an implementation.
        // Let's remove this for simplicity unless it's strictly needed.
        virtual std::istream& _read(std::istream& is);

        friend std::ofstream& operator<<(std::ofstream& ofs, const CanvasItem& item);
        friend std::ifstream& operator>>(std::ifstream& ifs, CanvasItem& item);
        virtual std::ofstream& _serialize(std::ofstream& ofs) const = 0;
        virtual std::ifstream& _deserialize(std::ifstream& ifs) = 0;

    public:
        inline static const float SELECTION_THRESHOLD = 4.0f; // pixels
    private:
        ID id = 0; // It's id location at the canvas.
    protected:
        Transform2D model{}; // Model transformation matrix
        TypeInfo typeInfo = TypeInfo::OTHER; // Retained as a single declaration
    };
}