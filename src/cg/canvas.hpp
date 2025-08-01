#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include "math.hpp"

#include "util.hpp"

#include "tool_box.hpp"

template <typename T>
using ArrayList = std::vector<T>; // alias the data structure by the name of what it actually is.

using DeltaTime = float;
using TimePoint = std::chrono::steady_clock::time_point;
using ID = std::size_t;


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

    }

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
    private:
        ID id = 0; // It's id location at the canvas.
    protected:
        Vector2 position{};
    };


    class Canvas {
        //friend class CanvasItem;
    public:
         Canvas(Vector2 window_size) {
            toolBox.addCanvas(this);
            windowSize = window_size;
            _screenToNdc = {
                { 2.0f / window_size.x, 0.0f, 0.0f },
                { 0.0f, -2.0f / window_size.y, 0.0f },
                { -1.0f, 1.0f, 1.0f },
            };
        }

        /** Send a screen input at screen coordinate.
         * Converts Screen Cordinates to Normalized Display Coordinates before trigger.
         */
        template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
        inline void sendScreenInput(int x, int y) {
            Vector2 position = screenToNdc(x, y);
            IE input_event{ position };
            sendInput(input_event);
        }

        template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
        inline void sendScreenInput(int x, int y, int direction) {
            Vector2 position = screenToNdc(x, y);
            IE input_event{ position, direction };
            sendInput(input_event);
        }

        /* Propagates user input to each Canvas Item on the canvas. */
        template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
        void sendInput(IE input_event) {
            // TODO -> Check if input is inside item area before sending event.
            for (auto& item : itens)
                item->_input(input_event);
        }
        void sendInput(io::MouseLeftButtonPressed input_event);
        void sendInput(io::MouseLeftButtonReleased input_event);
        void sendInput(io::MouseDrag input_event);

        /* Propagates a process call to each Canvas Item on the canvas. */
        TimePoint updateProcess(TimePoint lastTime);

        /* Propagates a render call to each Canvas Item on the canvas. */
        void updateRender();

        inline void insert(std::unique_ptr<CanvasItem> item) {
            item->id = itens.size();
            itens.push_back(std::move(item));
        }

        inline void remove(CanvasItem* item) {
            assert_err(!itens.empty() && item->id < itens.size(), "Invalid range.");

            size_t last = itens.size() - 1;
            if (item->id != last) {
                // atualiza os ids antes de trocar os ponteiros
                ID id = item->id;
                std::swap(item->id, itens[last]->id);
                std::swap(itens[id], itens[last]);
            }
            itens.pop_back(); // aqui o unique_ptr no fim do vetor é destruído e liberado do CanvasItem
        }

        ArrayList<CanvasItem *> select() { // futuramente expôr um modo de selecionar itens externamente
            ArrayList<CanvasItem *> refs{itens.size()}; // reserva
            for (auto& item : itens)
                refs.push_back(item.get());
            return refs;
        }

        inline Vector2 getWindowSize() {
            return windowSize;
        }

        // Apply screen coordinates to Normalized Display Coordinates at a point.
        inline Vector2 screenToNdc(Vector2 point) {
            return _screenToNdc.transform(point);
        }
        inline Vector2 screenToNdc(int x, int y) {
            return _screenToNdc.transform(x, y);
        }

    private:
        Vector2 windowSize; // aspect ratio: 10:7
        Transform2D _screenToNdc; // Screen coordinates to Normalized Display Coordinates

        ArrayList<std::unique_ptr<CanvasItem>> itens;
        CanvasItem* current = nullptr;
    public:
        ToolBox toolBox;
    };

}