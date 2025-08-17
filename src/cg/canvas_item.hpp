#pragma once

#include "math.hpp"

#include "input_event.hpp"


namespace cg {
    using ID = std::size_t;
    using DeltaTime = float;

    class CanvasItem {
        friend class Canvas;
    public:
        CanvasItem() = default;
        virtual ~CanvasItem() = default;
        CanvasItem(Vector2 position) : position{ position } {}

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
    private:
        ID id = 0; // It's id location at the canvas.
    public:
        Vector2 position{};
    };
}
