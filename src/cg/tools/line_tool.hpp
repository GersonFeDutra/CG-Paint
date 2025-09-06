#pragma once

#include <cg/tools.hpp>

#include "ghost_line.hpp"


namespace cg {
	class Line;

	class LineTool : public Painter {
		static constexpr Color GHOST_LINE_COLOR = { 0.15f, 0.35f, .75f, 0.66f };  // Azul claro semi-transparente
		static constexpr float DASH_LENGTH = 2.5f;  // Comprimento do tracejado em pixels
		static constexpr float GAP_LENGTH = 1.0f;    // Comprimento do espaço em pixels
	public:
		LineTool(ToolBox& tool_box);

		void _onRender() override;

		void _input(io::MouseMove mouse_event) override;
		void _input(io::MouseDrag mouse_event) override;
		void _input(io::MouseLeftButtonPressed mouse_event) override;
		void _input(io::MouseRightButtonPressed mouse_event) override;

		Vector2 getGhostLineStart() const;
		Vector2 getGhostLineEnd() const;
	private:
		GhostLine ghostLine;
		Line* line = nullptr;
	};
}
