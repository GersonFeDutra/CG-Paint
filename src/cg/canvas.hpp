#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include "util.hpp"
#include "math.hpp"

#include "input_event.hpp"
#include "tool_box.hpp"

#include "canvas_item.hpp"

template <typename T>
using ArrayList = std::vector<T>; // alias the data structure by the name of what it actually is.



namespace cg {
    using TimePoint = std::chrono::steady_clock::time_point;

    class Canvas {
    public:
         Canvas(Vector2 window_size) {
            toolBox.addCanvas(this);
            setWindowSize(window_size);
         }

        /** Send a screen input at screen coordinate.
         * Converts Screen Cordinates to Normalized Display Coordinates before trigger.
         */
        template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
        inline void sendScreenInput(int x, int y) {
            Vector2 position = screenToWorld(x, y);
            auto [_x, _y] = position;
            std::cout << "Mouse moved to: " << _x << ", " << _y << std::endl;

            IE input_event{ position };
            toolBox.captureInput(input_event);
        }

        template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
        inline void sendScreenInput(int x, int y, int direction) {
            Vector2 position = screenToWorld(x, y);
            IE input_event{ position, direction };
            toolBox.captureInput(input_event);
        }

        /* Propagates user input to a Canvas Item on the canvas. */
        template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
        inline void sendInput(IE input_event) {
            // TODO -> Check if input is inside item area before sending event.
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

        inline Vector2 getWindowSize() {
            return windowSize;
        }

        // Update coordinate system
        inline void setWindowSize(Vector2 to) {
            windowSize = to;
            _screenToWorld = {
                { 1.0f, 0.0f },
                { 0.0f, -1.0f },
                { -windowSize.x / 2.0f, windowSize.y / 2.0f },
			};
            // TODO -> Remove
            _screenToNdc = {
                { 2.0f / windowSize.x, 0.0f },
                { 0.0f, -2.0f / windowSize.y },
                { -1.0f, 1.0f },
            };
            _ndcToScreen = {
                { windowSize.x / 2.0f, 0.0f },
                { 0.0f, windowSize.y / -2.0f },
                { windowSize.x / 2.0f, windowSize.y / 2.0f },
            };
        }

        template <std::convertible_to<float> T>
        inline void setWindowSize(T width, T height) {
            setWindowSize({ (float)width, (float)height });
        }

        // Changes screen coordinates to World Coordinates system.
        inline Vector2 screenToWorld(Vector2 point) {
			return _screenToWorld.transform(point);
        }
        inline Vector2 screenToWorld(int x, int y) {
            return _screenToWorld.transform(x, y);
        }

        // Changes screen coordinates to Normalized Display Coordinates system.
        inline Vector2 screenToNdc(Vector2 point) {
            return _screenToNdc.transform(point);
        }
        inline Vector2 screenToNdc(int x, int y) {
            return _screenToNdc.transform(x, y);
        }

        // Changes Normalized Display Coordinates to Screen Coordinates system.
        inline Vector2 ndcToScreen(Vector2 point) {
            return _ndcToScreen.transform(point);
        }

        private:
        Vector2 windowSize; // aspect ratio: 10:7
		Transform2D _screenToWorld; // Screen coordinates to World coordinates
        Transform2D _screenToNdc; // Screen coordinates to Normalized Display Coordinates
        Transform2D _ndcToScreen; // Normalized Display Coordinates to Screen Coordinates

        ArrayList<std::unique_ptr<CanvasItem>> itens;
    public:

        // I'mma fucking kill myself
        inline const ArrayList<std::unique_ptr<CanvasItem>>& getItens() const {
            return itens;
        }
        
        ToolBox toolBox;
    };

}