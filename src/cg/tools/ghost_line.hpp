#pragma once

#include <cg/geometry.hpp>


namespace cg {
	class Canvas; // forward definition

	struct GhostLine {
		Vector2 *from = nullptr, *to = nullptr;
		Color color = DEFAULT_COLOR;

		GhostLine(Canvas* canvas);
		GhostLine(Vector2* from, Vector2* to, Canvas* canvas);
		GhostLine(Vector2* from, Vector2* to, Color color, Canvas* canvas);

		void _render();

	private:
		Canvas* canvas;
		static constexpr Color DEFAULT_COLOR = { 0.15f, 0.35f, .75f, 0.66f }; // Azul claro semi-transparente
		static constexpr float DASH_LENGTH = 2.5f; // Comprimento do tracejado em pixels
		static constexpr float GAP_LENGTH = 1.0f;  // Comprimento do espaçamento em pixels
	};

}