#pragma once

#include "../tools.hpp"
#include "../canvas.hpp"


namespace cg {

    class SelectTool : public Tool {
    public:
        SelectTool(ToolBox& tool_box) : Tool(tool_box) {}
        void _render() override {
            if (selectedItem) {
                // Pode desenhar highlight aqui (ex: bounding box ou cor diferente)
            }
        }

        void _input(io::MouseLeftButtonPressed mouse_event) override {
            pick(toolBox.canvas->getItens(), mouse_event.position);
        }

        void _input(io::MouseDrag mouse_event) override {
            if (selectedItem) {
                // Exemplo simples: mover o item inteiro
                selectedItem->translateTo(mouse_event.position);
            }
        }

        void pick(const std::vector<std::unique_ptr<CanvasItem>>& items, Vector2 mousePos) {
            selectedItem = nullptr;
            for (auto& item : items) {
                if (item->isSelected(mousePos)) {
                    selectedItem = item.get();
                    break; // pega o primeiro encontrado
                }
            }
        }

        inline void translateSelected(const Vector2& delta) {
            if (selectedItem)
                selectedItem->translate(delta);
		}

    // setters e getters
        void setPosition(const Vector2& position) override {
			translateSelected(position - _getPositionRef()); // delta △translation
			Tool::setPosition(position);
        }

        inline void select(CanvasItem* item) {
			selectedItem = item;
            model = Transform2D(item->model * Vector2{});
        }

        inline bool hasSelection() const {
            return selectedItem != nullptr;
        }

    private:
        CanvasItem* selectedItem = nullptr;
    };

}