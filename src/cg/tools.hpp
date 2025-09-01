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
		std::ostream& _print(std::ostream& os) const override { return os; }
		std::ofstream& _serialize(std::ofstream& ofs) const override { return ofs; }
		std::ifstream& _deserialize(std::ifstream& ifs) override { return ifs; }

	protected:
		ToolBox& toolBox;
	};

	/* Ferramenta que permite criar e modelar itens de Canvas. */
	class Painter : public Tool {
	public:
		Painter(ToolBox& tool_box) : Tool{tool_box} {}

		void appendToCanvas(CanvasItem* item);

		void _render() override;
		virtual void _onRender() {};
	protected:
		bool isDrawing = false; // _render only if isDrawing
	};

}