// #pragma once

// #include "../tools.hpp"
// #include "../canvas.hpp"

// namespace cg {

//     class SelectTool : public Tool {
//     public:
//         SelectTool(ToolBox& tool_box) : Tool(tool_box) {}

//         void _render() override {
//             if (selectedItem) {
//                 // Pode desenhar highlight aqui
//             }
//         }

//         void _input(io::MouseLeftButtonPressed mouse_event) override {
//             pick(toolBox.canvas->getItens(), mouse_event.position);
//         }

//         void _input(io::MouseDrag mouse_event) override {
//             if (selectedItem) {
//                 selectedItem->translateTo(mouse_event.position);
//             }
//         }

//         void pick(const std::vector<std::unique_ptr<CanvasItem>>& items, Vector2 mousePos) {
//             selectedItem = nullptr;
//             for (auto& item : items) {
//                 if (item->isSelected(mousePos)) {
//                     select(item.get());
//                     return;
//                 }
//             }
//             deselect();
//         }

//         inline void select(CanvasItem* item) {
//             selectedItem = item;
//             model = Transform2D(item->model * Vector2{});

//             // Vincula a cor do item selecionado na ToolBox para edição
//             toolBox.bindColorPtr(&item->getColor(), item);
//         }

//         inline void deselect() {
//             selectedItem = nullptr;
//             toolBox.unbindColorPtr();
//         }

//         inline void translateSelected(const Vector2& delta) {
//             if (selectedItem)
//                 selectedItem->translate(delta);
//         }

//         inline void rotateSelected(float angle) {
//             if (selectedItem)
//                 selectedItem->rotate(angle);
//         }

//         // setters e getters
//         inline bool hasSelection() const {
//             return selectedItem != nullptr;
//         }

//         inline CanvasItem* getSelectedItem() const {
//             return selectedItem;
//         }

//         void setPosition(const Vector2& position) override {
//             translateSelected(position - getPosition());
//             Tool::setPosition(position);
//         }

//         void setRotation(float angle) override {
//             rotateSelected(angle - getRotation());
//             Tool::setRotation(angle);
//         }

//     private:
//         CanvasItem* selectedItem = nullptr;
//     };

// }

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
                selectedItem->setPosition(mouse_event.position);
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

    private:
        CanvasItem* selectedItem = nullptr;
    };

}