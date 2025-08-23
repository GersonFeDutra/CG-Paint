#include <memory>

#include <util.hpp>
#include <math.h>

#include "line_tool.hpp"

#include <cg/tool_box.hpp>
#include <cg/canvas_itens/line.hpp>


namespace cg {
	LineTool::LineTool(ToolBox& tool_box) : Painter{ tool_box }, ghostLine{ toolBox.canvas } {
		ghostLine.from = ghostLine.to = &getPosition(); // Start both `from` and `to` point as the current position
	}

	void LineTool::_onRender() {
		ghostLine._render();
	}

	void LineTool::_input(io::MouseMove mouse_event)
	{
		if (isDrawing)
			setPosition(mouse_event.position);
	}

	void LineTool::_input(io::MouseDrag mouse_event)
	{
		if (isDrawing)
			setPosition(mouse_event.position);
	}


	void LineTool::_input(io::MouseLeftButtonPressed mouse_event)
	{
		// ghost line `from` aways points to `position`
		// ghost line `to` aways points to the current line's `lastVertice`
		setPosition(mouse_event.position); // update position to the first vertice

		if (isDrawing) {
			line->append(mouse_event.position);
			ghostLine.from = &(line->lastVertice());
		}
		else {
			// New line primitive
			line = new Line(mouse_event.position, toolBox.getColor());
			appendToCanvas(line);
			toolBox.bindColorPtr(&line->getColor());

			ghostLine.from = &line->lastVertice();
			isDrawing = true;
		}
	}

	// TODO -> Accept with Enter
	// TODO -> Cancel with Escape
	void LineTool::_input(io::MouseRightButtonPressed mouse_event) {
		isDrawing = false;

		// TODO -> fallback to point if only one vertice
		if (line != nullptr && line->size() < 2)
			delete line; // we do not make a line with single vertice

		line = nullptr;
		ghostLine.from = &getPosition(); // reset ghost line `from` to the current position
		toolBox.unbindColorPtr();
	}

}