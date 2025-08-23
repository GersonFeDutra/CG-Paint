#pragma once 

#include "canvas_item.hpp"


namespace cg {
	class ToolBox;

	/* Classe base para ferramentas como Painters e Selectors */
	class Tool : public CanvasItem {
	public:
		Tool(ToolBox& tool_box) : toolBox{ tool_box } {}
	protected:
		ToolBox& toolBox;

		// Inherited via CanvasItem
		std::ostream& _print(std::ostream& os) const override { return os; }
		std::ofstream& _serialize(std::ofstream& ofs) const override { return ofs; }
		std::ifstream& _deserialize(std::ifstream& ifs) override { return ifs; }
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

	class Selector : public Tool {
		CanvasItem* selectedItem = nullptr;

		void pick(const std::vector<CanvasItem*>& items, Vector2 mousePos);

		bool hasSelection() const {
			return selectedItem != nullptr;
		}
	};

}