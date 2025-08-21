#include <memory>

#include <util.hpp>
#include <math.h>

#include "line_tool.hpp"

#include <cg/tool_box.hpp>
#include <cg/canvas_itens/line.hpp>


namespace cg {
    LineTool::LineTool(ToolBox& tool_box) : Painter{ tool_box }, ghostLine{ toolBox.canvas } {
        ghostLine.from = ghostLine.to = &position;
    }

    void LineTool::_onRender() {
        ghostLine._render();
    }

    void LineTool::_input(io::MouseMove mouse_event)
    {
        if (isDrawing)
            position = mouse_event.position;
    }

    void LineTool::_input(io::MouseDrag mouse_event)
    {
        if (isDrawing)
            position = mouse_event.position;
    }


    void LineTool::_input(io::MouseLeftButtonPressed mouse_event)
    {
        // ghost line `from` aways points to `position`
        // ghost line `to` aways points to the current line's `lastVertice`

        if (isDrawing) {
            if (line->size() == 0)
                appendToCanvas(line); // add to canvas when line vertices become > 1

            line->append(mouse_event.position);
            ghostLine.from = &(line->lastVertice());
        }
        else {
            // New line primitive
            line = new Line(mouse_event.position, toolBox.getColor());
            toolBox.bindColorPtr(&line->getColor());
			position = mouse_event.position; // update position to the first vertice
            ghostLine.from = &line->lastVertice();
            isDrawing = true;
        }
    }

    // TODO -> Accept with Enter
    // TODO -> Cancel with Escape
    void LineTool::_input(io::MouseRightButtonPressed mouse_event) {
        isDrawing = false;

        if (line != nullptr && line->size() == 0)
            delete line; // do not make a line with single vertice

        line = nullptr;
        ghostLine.from = &position;
        toolBox.unbindColorPtr();
    }

}