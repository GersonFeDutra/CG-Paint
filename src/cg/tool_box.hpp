#pragma once

#include "math.hpp"

namespace cg {
	class Canvas;
	class CanvasItem;

	/** Manages the current tools used in the Canvas.
	 * Allows to create and manage CanvasItens.
	 */
	class ToolBox {
	public:
		void addCanvas(Canvas* canvas_ptr);
		CanvasItem *newPrimitive(Vector2 at);
    public:
		enum Primitives {
			POINT = 0,
			LINE = 1,
			POLYGON = 2,
		};
        int currentPrimitive;
	public:
		Color currentColor = cg::colors::WHITE; // Define a cor atual para pintura.
	private:
		Canvas* canvas = nullptr;
	};
}