#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include "math.hpp" // Vector2i

#include "util.hpp"


template <typename T>
using ArrayList = std::vector<T>; // alias the data structure by the name of what it actually is.

using DeltaTime = float;
using TimePoint = std::chrono::steady_clock::time_point;
using ID = std::size_t;


namespace cg {

    namespace io {

        struct MouseInputEvent {
            Vector2i position; // mouse position on screen
        };
        using MouseMove = MouseInputEvent;
        struct MouseDrag : public MouseInputEvent {};
        struct MouseRightButtonPressed : public MouseInputEvent {};
        struct MouseRightButtonReleased : public MouseInputEvent {};
        struct MouseLeftButtonPressed : public MouseInputEvent {};
        struct MouseLeftButtonReleased : public MouseInputEvent {};
        struct MouseWheelV : public MouseInputEvent {
            int direction; // direction moved △y
        };
        struct MouseWheelH : public MouseInputEvent {
            int direction; // direction moved △x
        };

    }

    class CanvasItem {
        friend class Canvas;
    public:
        CanvasItem() = default;
        virtual ~CanvasItem() = default;

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
    };


    class Canvas {
        //friend class CanvasItem;
    public:
        /* Propagates user input to each Canvas Item on the canvas. */
        template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
        void sendInput(IE input_event) {
            // TODO -> Check if input is inside item area before sending event.
            for (auto& item : itens)
                item->_input(input_event);
        }

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
    private:
        ArrayList<std::unique_ptr<CanvasItem>> itens;
    };

}