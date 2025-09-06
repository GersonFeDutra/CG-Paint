#pragma once

#include <memory>
#include <vector>
#include <chrono>

#include "util.hpp"
#include "math.hpp"

#include "input_event.hpp"
#include "tool_box.hpp"

#include "canvas_item.hpp"


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
        template <typename IE> requires std::is_base_of_v<io::InputEvent, IE>
        inline void sendScreenInput(int x, int y) {
            Vector2 position = screenToWorld(x, y);
            IE input_event{ position };
            toolBox.captureInput(input_event);
        }

        template <typename IE> requires std::is_base_of_v<io::InputEvent, IE>
        inline void sendScreenInput(int x, int y, int direction) {
            Vector2 position = screenToWorld(x, y);
            IE input_event{ position, direction };
            toolBox.captureInput(input_event);
        }

		template <typename KIE> requires std::is_base_of_v<io::KeyInputEvent, KIE>
        inline void sendScreenInput(int x, int y, int key, int mods) {
            Vector2 position = screenToWorld(x, y);
            KIE input_event{ position, key, mods };
            toolBox.captureInput(input_event);
        }

		template <typename FIE> requires std::is_base_of_v<io::FocusInputEvent, FIE>
        inline void sendScreenInput() {
            FIE input_event;
            toolBox.captureInput(input_event);
        }

        /* Propagates user input to a Canvas Item on the canvas. */
        template <typename IE> requires std::is_base_of_v<io::InputEvent, IE>
        inline void sendInput(IE input_event) {
            // TODO -> Check if input is inside item area before sending event.
        }

        /* Propagates a process call to each Canvas Item on the canvas. */
        TimePoint updateProcess(TimePoint lastTime);

        /* Propagates a render call to each Canvas Item on the canvas. */
        void updateRender();

        inline void insert(std::unique_ptr<CanvasItem> item) {
            item->id = itens.size();

            // incrementa o contador de tipos
            if ((int)item->getTypeInfo() < (int)CanvasItem::TypeInfo::OTHER)
			    typeCount[(int)item->getTypeInfo()]++;

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
            
            // decrementa o contador de tipos
            if ((int)item->getTypeInfo() < (int)CanvasItem::TypeInfo::OTHER)
			    typeCount[(int)itens.back()->getTypeInfo()]--;

            itens.pop_back(); // aqui o unique_ptr no fim do vetor é destruído e liberado do CanvasItem
        }

        /** Retorna o primeiro ítem encontrado na posição passada.
         * Se não for encontrado, retorna `nullptr`
         */
        CanvasItem* pick(Vector2 mouse_position) {
            for (auto it = itens.rbegin(); it != itens.rend(); ++it) {
                const auto& item = *it;
                if (item->isSelected(mouse_position))
                    return item.get();
            }
            return nullptr;
        }

        inline Vector2 getWindowSize() const {
            return windowSize;
        }

        inline float getAspectRatio() const {
            return windowSize.x / windowSize.y;
		}

        inline Vector2 getUpperLeft() const {
            return screenToWorld(0, 0);
		}

        inline Vector2 getBottomRight() const {
            return screenToWorld((int)windowSize.x, (int)windowSize.y);
        }

        // Update coordinate system
        inline void setWindowSize(Vector2 to) {
            windowSize = to;
            _screenToWorld = {
                { 1.0f, 0.0f },
                { 0.0f, -1.0f },
                { -windowSize.x / 2.0f, windowSize.y / 2.0f },
			};
            /*_screenToNdc = {
                { 2.0f / windowSize.x, 0.0f },
                { 0.0f, -2.0f / windowSize.y },
                { -1.0f, 1.0f },
            };
            _ndcToScreen = {
                { windowSize.x / 2.0f, 0.0f },
                { 0.0f, windowSize.y / -2.0f },
                { windowSize.x / 2.0f, windowSize.y / 2.0f },
            };*/
            for (auto& item : itens)
                item->_reshape(*this);
            toolBox._reshape(*this);
        }

        template <std::convertible_to<float> T>
        inline void setWindowSize(T width, T height) {
            setWindowSize({ (float)width, (float)height });
        }

        // Changes screen coordinates to World Coordinates system.
        inline Vector2 screenToWorld(Vector2 point) const {
			return _screenToWorld.transform(point);
        }
        inline Vector2 screenToWorld(int x, int y) const {
            return _screenToWorld.transform(x, y);
        }

        inline size_t getTypeCount(CanvasItem::TypeInfo of_type) {
			return typeCount[(int)of_type];
        }

        inline void clear() {
            // WARNING -> Cuidado, clear pode remover ferramentas internas além das primitivas!
            itens.clear();
            for (int i = 0; i < 3; ++i) // reset typeCount
				typeCount[i] = 0;
        }

        inline const ArrayList<std::unique_ptr<CanvasItem>>& getItens() const {
            return itens;
        }

        CanvasItem *hitTest(float mx, float my);
    private:
        Vector2 windowSize; // aspect ratio: 10:7
		Transform2D _screenToWorld; // Screen coordinates to World coordinates
        //Transform2D _screenToNdc; // Screen coordinates to Normalized Display Coordinates
        //Transform2D _ndcToScreen; // Normalized Display Coordinates to Screen Coordinates

        ArrayList<std::unique_ptr<CanvasItem>> itens;
        size_t typeCount[3];
    public:

        ToolBox toolBox;
    };

}
