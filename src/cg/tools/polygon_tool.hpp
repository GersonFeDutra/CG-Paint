#pragma once

#include <memory>

#include <cg/tools.hpp>

#include "ghost_line.hpp"


namespace cg {
	class Polygon;
    class Line;

	class PolygonTool : public Painter
	{
	public:
		PolygonTool(ToolBox& tool_box);

		void _onRender() override;

		void _input(io::MouseMove mouse_event) override;
		void _input(io::MouseDrag mouse_event) override;
		void _input(io::MouseLeftButtonPressed mouse_event) override;
        void _input(io::MouseRightButtonPressed mouse_event) override;
	private:
		Polygon* polygon; // dummie
		GhostLine lines[2];
        Vector2 initialCoords;
	};
}