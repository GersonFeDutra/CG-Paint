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

	// TODO
	class Selector : public Tool {};

}