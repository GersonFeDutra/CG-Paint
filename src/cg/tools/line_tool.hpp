#pragma once

#include <cg/tools.hpp>


namespace cg {
	class Line;

	class LineTool : public Painter {
	public:
		LineTool(ToolBox& tool_box) : Painter{ tool_box } {}
		CanvasItem* make(Vector2 at) override;

		void _render() override;

		void _input(io::MouseMove mouse_event) override;
		void _input(io::MouseLeftButtonPressed mouse_event) override;
	private:
		Line* line = nullptr;
	};
}