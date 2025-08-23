#pragma once

namespace cg {

    namespace io {
        class Canvas;

        struct InputEvent {
            Vector2 position; // mouse position on screen
            // Transforms mouse Screen Coordinates to Normalized Device Coordinates before creating the event.
            InputEvent(Vector2 position) : position{ position } {}
		};

        struct MouseInputEvent : InputEvent {
            MouseInputEvent(Vector2 position) : InputEvent{ position } {}
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

        struct KeyInputEvent : InputEvent {
			int key; // key code (ASCII if keyboard, or GLUT Enum if special)
            int mods; // bit field describing which modifier keys were held down
            KeyInputEvent(Vector2 position, int key, int mods) : InputEvent{ position }, key{ key }, mods{ mods } {}
		};

        struct KeyboardInputEvent : KeyInputEvent {
            KeyboardInputEvent(Vector2 position, int key, int mods) : KeyInputEvent{ position, key, mods } {}
        };

        struct SpecialKeyInputEvent : KeyInputEvent {
            SpecialKeyInputEvent(Vector2 position, int code, int mods) : KeyInputEvent{ position, code, mods } {}
		};

    } // namespace io

} // namespace cg