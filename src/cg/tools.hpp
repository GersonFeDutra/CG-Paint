#pragma once 

#include "canvas_item.hpp"


namespace cg {
	class ToolBox;

	/* Classe base para ferramentas como Painters e Selectors */
	class Tool : public CanvasItem {
	// construtores
	public:
		Tool(ToolBox& tool_box) : toolBox{ tool_box } {}
	
	// setters e getters
	public:
		// Define a posição absoluta do "scanner" da ferramenta no canvas.
		// Pode ser usado para desenhar o cursor da ferramenta e posicionar novos itens.
		virtual void setPosition(const Vector2& position) {
			model.moveTo(position); // Dado que a matriz é identidade, move para a posição absoluta.
		}

		Vector2 getPosition() const {
			return model.columns[2];
		}
	protected:
		// Retorna a posição absoluta do "scanner" da ferramenta no canvas.
		Vector2& _getPositionRef() {
			return model.columns[2];
		}

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