#pragma once

#include "math.hpp"

#include "input_event.hpp"


namespace cg {
    using ID = std::size_t;
    using DeltaTime = float;

    class CanvasItem {
        friend class Canvas;
    public:
        enum class TypeInfo {
            POINT = 0,
            LINE,
            POLYGON,
            OTHER,
        };
    public:
		CanvasItem() = default;
        CanvasItem(TypeInfo type_info) : typeInfo{type_info} {}
        virtual ~CanvasItem() = default;
        CanvasItem(TypeInfo type_info, Vector2 position) : typeInfo{ type_info }, position{ position } {}

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

        /** Process data between variations of delta time △t.
         * @param delta : time stamp between previous frame
         */
        virtual void _process(DeltaTime delta) {}
        /* Draw data onto screen with open GL calls. */
        virtual void _render() {}

        inline void setPosition(Vector2 pos) {
            position = pos;
        }
        
        inline TypeInfo getTypeInfo() const {
            return typeInfo;
        }

        friend std::ostream& operator<<(std::ostream& os, const CanvasItem& item);
        friend std::istream& operator>>(std::istream& is, CanvasItem& item);
        virtual std::ostream& _print(std::ostream& os) const = 0;
        virtual std::istream& _read(std::istream& is);

        friend std::ofstream& operator<<(std::ofstream& ofs, const CanvasItem& item);
        friend std::ifstream& operator>>(std::ifstream& ifs, CanvasItem& item);
        virtual std::ofstream& _serialize(std::ofstream& ofs) const = 0;
		virtual std::ifstream& _deserialize(std::ifstream& ifs) = 0;

    private:
        ID id = 0; // It's id location at the canvas.
        TypeInfo typeInfo = TypeInfo::OTHER; // Type info for later serialization
    public:
        Vector2 position{}; // TODO -> Remove + use model transform matrix
    };
}
