#pragma once

#include <functional>
#include <cg/math.hpp>
#include <cg/geometry.hpp>


namespace cg {
	class Canvas; // forward definition

	struct GhostLine {
		std::function<Vector2()> getFrom;
		std::function<Vector2()> getTo;

		Color color = DEFAULT_COLOR;

		GhostLine(Canvas* canvas);
		GhostLine(std::function<Vector2()> getFrom, std::function<Vector2()> getTo, Canvas* canvas);
		GhostLine(std::function<Vector2()> getFrom, std::function<Vector2()> getTo, Color color, Canvas* canvas);

		void _render();

	private:
		Canvas* canvas;
		static constexpr Color DEFAULT_COLOR = { 0.15f, 0.35f, .75f, 0.66f }; // Azul claro semi-transparente
		static constexpr float DASH_LENGTH = 2.5f; // Comprimento do tracejado em pixels
		static constexpr float GAP_LENGTH = 1.0f;  // Comprimento do espaçamento em pixels
	};

}