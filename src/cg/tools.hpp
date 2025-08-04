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

		virtual CanvasItem* make(Vector2 at) = 0;
	protected:
		bool isDrawing = false;
	};

	// TODO
	class Selector : public Tool {};

}