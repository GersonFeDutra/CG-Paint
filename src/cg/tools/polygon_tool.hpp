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

		Vector2 getFirstGhostLineStart() const;
		Vector2 getLastGhostLineStart() const;

		Vector2 getGhostLineEnd() const;
	private:
		Polygon* polygon = nullptr; // current polygon being drawn
		GhostLine lines[2];
        Vector2 initialCoords;
	};

}
