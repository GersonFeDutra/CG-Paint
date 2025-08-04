#pragma once

#include <cg/tools.hpp>

#include <cg/canvas_itens/point.hpp>


namespace cg {
	class Point;
	
	/* Ferramenta para criação de pontos. */
	class PointTool : public Painter {
	public:
		PointTool(ToolBox& tool_box);
		CanvasItem* make(Vector2 at) override;

		void _render() override;

		void _input(io::MouseMove mouse_event) override;
		void _input(io::MouseDrag mouse_event) override;
		void _input(io::MouseLeftButtonPressed mouse_event) override;
		void _input(io::MouseLeftButtonReleased mouse_event) override;
	private:
		Point* point = nullptr;
	};

}