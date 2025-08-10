#pragma once

#include <cg/tools.hpp>


namespace cg {
	class Line;

	class LineTool : public Painter {
		const Color GHOST_LINE_COLOR = { 0.15f, 0.35f, .75f, 0.66f };  // Azul claro semi-transparente
		const float DASH_LENGTH = 2.5f;  // Comprimento do tra�ejado em pixels
		const float GAP_LENGTH = 1.0f;    // Comprimento do espa�o em pixels
	public:
		LineTool(ToolBox& tool_box) : Painter{ tool_box } {}
		CanvasItem* make(Vector2 at) override;

		void _render() override;

		void _input(io::MouseMove mouse_event) override;
		void _input(io::MouseDrag mouse_event) override;
		void _input(io::MouseLeftButtonPressed mouse_event) override;
	private:
		Line* line = nullptr;
		std::vector<Line *> lines; // added for testing
	};
}