#pragma once

namespace cg {

    namespace io {
        class Canvas;

        struct MouseInputEvent {
            Vector2 position; // mouse position on screen

            // Transforms mouse Screen Coordinates to Normalized Device Coordinates before creating the event.
            MouseInputEvent(Vector2 position) : position{ position } {}
        };
        using MouseMove = MouseInputEvent;
        struct MouseDrag : public MouseInputEvent {
            MouseDrag(Vector2 position) : MouseInputEvent{ position } {}
        };
        struct MouseRightButtonPressed : public MouseInputEvent {
            MouseRightButtonPressed(Vector2 position) : MouseInputEvent{ position } {}
        };
        struct MouseRightButtonReleased : public MouseInputEvent {
            MouseRightButtonReleased(Vector2 position) : MouseInputEvent{ position } {}
        };
        struct MouseLeftButtonPressed : public MouseInputEvent {
            MouseLeftButtonPressed(Vector2 position) : MouseInputEvent{ position } {}
        };
        struct MouseLeftButtonReleased : public MouseInputEvent {
            MouseLeftButtonReleased(Vector2 position) : MouseInputEvent{ position } {}
        };
        struct MouseWheelV : public MouseInputEvent {
            int direction; // direction moved △y

            MouseWheelV(Vector2 position, int direction) : MouseInputEvent{ position }, direction{ direction } {}
        };
        struct MouseWheelH : public MouseInputEvent {
            int direction; // direction moved △x

            MouseWheelH(Vector2 position, int direction) : MouseInputEvent{ position }, direction{ direction } {}
        };

    } // namespace io

} // namespace cg