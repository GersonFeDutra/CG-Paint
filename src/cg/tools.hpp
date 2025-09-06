#pragma once 

#include "canvas_item.hpp"


namespace cg {
	class ToolBox;

	/* Classe base para ferramentas como Painters e Selectors */
	class Tool : public CanvasItem {
	// construtores
	public:
		Tool(ToolBox& tool_box) : toolBox{ tool_box } {}
	
	public:
	// setters e getters
	
		// Define a posição absoluta do "scanner" da ferramenta no canvas.
		// Pode ser usado para desenhar o cursor da ferramenta e posicionar novos itens.
		virtual void setPosition(const Vector2& position) {
			model.setOrigin(position);
		}

		virtual void setRotation(float angle) {
			model.rotateTo(angle);
		}

		virtual void setScale(const Vector2& scale) {
			model.scaleTo(scale);
		}

		// Retorna a posição absoluta do "scanner" da ferramenta no canvas.
		inline Vector2 getPosition() const {
			return model.getOrigin();
		}

		// Retorna a rotação da ferramenta no canvas.
		inline float getRotation() const {
			return model.getRotation();
		}

		// Retorna a escala da ferramenta no canvas.
		inline Vector2 getScale() const {
			return model.getScale();
		}

	protected:
	// Inherited via CanvasItem
		bool _isSelected(Vector2 cursor_local_position) const override { return false; }

	// operadores virtuais
		//std::ostream& _print(std::ostream& os) const override { return os; }
		std::ostream& _serialize(std::ostream& os) const override { return os; }
		std::istream& _deserialize(std::istream& is) override { return is; }

	protected:
		ToolBox& toolBox;
	};

	/* Ferramenta que permite criar e modelar itens de Canvas. */
	class Painter : public Tool {
	public:
		Painter(ToolBox& tool_box) : Tool{tool_box} {}

		void appendToCanvas(CanvasItem* item);

        void _input(io::MouseMove mouse_event) override {
			Tool::_input(mouse_event);
		}
        void _input(io::MouseDrag mouse_event) override {
			Tool::_input(mouse_event);
		}
        void _input(io::MouseRightButtonPressed mouse_event) override {
			Tool::_input(mouse_event);
		}
        void _input(io::MouseLeftButtonPressed mouse_event) override {
			Tool::_input(mouse_event);
		}
        void _input(io::MouseRightButtonReleased mouse_event) override {
			Tool::_input(mouse_event);
		}
        void _input(io::MouseLeftButtonReleased mouse_event) override {
			Tool::_input(mouse_event);
		}
        void _input(io::MouseWheelV mouse_event) override {
			Tool::_input(mouse_event);
		}
        void _input(io::MouseWheelH mouse_event) override {
			Tool::_input(mouse_event);
		}
		void _input(io::KeyInputEvent key_event) override {
			Tool::_input(key_event);
		}

		void _input(io::FocusIn _focus_event) override {
			_drawing |= (std::byte)IS_FOCUSED;
		}
		void _input(io::FocusOut _focus_event) override {
			_drawing &= ~(std::byte)IS_FOCUSED;
		}

		void _render() override;

		virtual void _onRender() {}

		inline void enableDraw() {
			_drawing |= (std::byte)IS_DRAWING;
		}
		inline void disableDraw() {
			_drawing &= ~(std::byte)IS_DRAWING;
		}
		inline bool isDrawing() {
			return _drawing == (std::byte)(IS_DRAWING | IS_FOCUSED);
		}

	protected:
		enum Drawing {
			NOT_DRAWING = 0,
			IS_DRAWING = 1,
			IS_FOCUSED = 2,
		};
		std::byte _drawing = (std::byte)IS_FOCUSED; // _render only if _drawing
	};

}
