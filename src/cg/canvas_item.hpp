#pragma once

#include "math.hpp"

#include "input_event.hpp"


namespace cg {
    using ID = std::size_t;
    class Canvas;

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
        CanvasItem(TypeInfo type_info) : typeInfo{type_info} {}
        virtual ~CanvasItem() = default;
        CanvasItem(TypeInfo type_info, Vector2 position) : typeInfo{ type_info }, model(position) {}

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

		virtual void _input(io::FocusIn _focus_event) {}
		virtual void _input(io::FocusOut _focus_event) {}

        /** Process data between variations of delta time △t.
         * @param delta : time stamp between previous frame
         */
        virtual void _process(DeltaTime delta) {}
        /* Draw data onto screen with open GL calls. */
        virtual void _render() {}

        /* Reshape/ resize window event. Use canvas.getWindowSize to update the geometry. */
        virtual void _reshape(Canvas& canvas) {}

        // verificar se mouse está dentro do item
        inline bool isSelected(Vector2 mouse_position) const { return _isSelected(toLocal(mouse_position)); }

    protected:
        virtual bool _isSelected(Vector2 cursor_local_position) const = 0;

    public:
        virtual inline void translate(Vec2Offset by) {
            model.translate(by);
		}

        virtual inline void rotate(Angle by) {
            model.rotate(by);
        }

        virtual inline void scale(Vector2 by) {
            model.scale(by);
        }

        inline void translateTo(Vector2 to) {
            model.translateTo(to);
		}

        inline void rotateTo(float angle) {
            model.rotateTo(angle);
        }
        
        inline TypeInfo getTypeInfo() const {
            return typeInfo;
        }

		// Converts position from Global Coordinate System to the Local Coordinate System (model) of the item
        inline Vector2 toLocal(Vector2 global_position) const {
            // Applies the inverse of the model transformation to the global position
            return model.inverse() * global_position;
        }

        inline Vector2 toGlobal(Vector2 local_position) const {
            // Applies the model transformation to the local position obtainning a global coordinate.
            return model * local_position;
        }

        friend std::ostream& operator<<(std::ostream& os, const CanvasItem& item);
        friend std::istream& operator>>(std::istream& is, CanvasItem& item);
        virtual std::ostream& _print(std::ostream& os) const = 0;
        virtual std::istream& _read(std::istream& is);

        friend std::ofstream& operator<<(std::ofstream& ofs, const CanvasItem& item);
        friend std::ifstream& operator>>(std::ifstream& ifs, CanvasItem& item);
        virtual std::ofstream& _serialize(std::ofstream& ofs) const = 0;
		virtual std::ifstream& _deserialize(std::ifstream& ifs) = 0;

    public:
        inline static const float SELECTION_THRESHOLD = 4.0f; // pixels
    private:
        ID id = 0; // It's id location at the canvas.
        TypeInfo typeInfo = TypeInfo::OTHER; // Type info for later serialization
    protected:
        // Use this to avoid serializing data. Useful when you inherits basic primitives, e.g. tools, like guidelines.
        inline void noSerialize() {
            typeInfo = TypeInfo::OTHER;
        }
		Transform2D model{}; // Model transformation matrix
    };
}
