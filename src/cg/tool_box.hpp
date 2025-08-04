#pragma once

#include <array>

#include "math.hpp"
#include "input_event.hpp"


namespace cg {
	// Foward Declarations
	class Canvas;
	class Painter;
	class PointTool;
	class LineTool;

	/** Gerencia as ferramentas atuais usadas no Canvas.
	 * Simplesmente delega os eventos de entrada para a ferramenta atual.
	 */
	class ToolBox {
		friend class Tool;
		static constexpr std::size_t N_PRIMITIVES = 2;
	public:
		ToolBox();

		void addCanvas(Canvas* canvas_ptr);

		void _render();

		template <typename IE> requires std::is_base_of_v<io::MouseInputEvent, IE>
		void captureInput(IE input_event) {}

		void captureInput(io::MouseMove input_event);
		void captureInput(io::MouseDrag input_event);
		void captureInput(io::MouseLeftButtonPressed input_event);
		void captureInput(io::MouseLeftButtonReleased input_event);
    public:
		enum Primitives {
			POINT = 0,
			LINE = 1,
			POLYGON = 2,
		};
        int currentPrimitive = POINT;
	public:
		// TODO -> Cor secundária [alternada com x key]
		Color currentColor = cg::colors::WHITE; // Define a cor atual para pintura.
		// TODO -> Polygon Tool
		int polygonEdges = 1;

		Canvas* canvas = nullptr;
	private:
		std::array<Painter *, N_PRIMITIVES> tools;
		int currentTool = POINT;
	};

}