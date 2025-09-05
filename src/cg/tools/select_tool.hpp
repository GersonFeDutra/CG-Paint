#pragma once

#include "../tools.hpp"
#include "../canvas.hpp"
#include "gizmo.hpp"


namespace cg {

    class SelectTool : public Tool {
    public:
        SelectTool(ToolBox& tool_box) : Tool(tool_box) {}
        void _render() override {
            if (selectedItem) {
                // Pode desenhar highlight aqui (ex: bounding box ou cor diferente)
            }
        }

        void _input(io::MouseMove mouse_event) override;

        void _input(io::MouseLeftButtonPressed mouse_event) override {
            if (CanvasItem* item = toolBox.canvas->pick(mouse_event.position)) {
                select(item);
                item->_input(mouse_event);
            }
        }

        void _input(io::MouseDrag mouse_event) override {
            if (selectedItem)
                selectedItem->_input(mouse_event); // Repassa o evento para o ítem selecionado
        }

        void select(CanvasItem* item);

        inline void translateSelected(const Vec2Offset& delta) {
            if (selectedItem)
                selectedItem->translate(delta);
		}

        inline void rotateSelected(DeltaAngle angle) {
            if (selectedItem)
                selectedItem->rotate(angle);
		}

        // Scale by delta △scale
        inline void scaleSelected(Vector2 by) {
            if (selectedItem)
                selectedItem->scale(by);
        }

    // setters e getters
        inline bool hasSelection() const {
            return selectedItem != nullptr;
        }

        void setPosition(const Vector2& position) override {
			translateSelected(position - getPosition()); // delta △translation
			Tool::setPosition(position);
        }

        void setRotation(float angle) override {
            if (selectedItem)
                selectedItem->rotateTo(angle);

			Tool::setRotation(angle);
        }

        inline void scale(Vector2 by) override {
            scaleSelected(by);
            Tool::scale(by);
        }

        inline void rotate(float by) override {
            rotateSelected(by); // delta △rotation
            Tool::rotate(by);
        }

        inline void translate(Vec2Offset by) override {
            translateSelected(by);
            Tool::translate(by);
        }

        void setScale(const Vector2& scale) override {
            auto [x, y] = getScale();
            scaleSelected({ scale.x / x, scale.y / y });
			Tool::setScale(scale);
        }

    private:
        CanvasItem* selectedItem = nullptr;
    };

}