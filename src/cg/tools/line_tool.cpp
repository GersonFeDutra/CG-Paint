#include <memory>
#include <util.hpp>
#include <math.h>

#include "line_tool.hpp"

#include "select_tool.hpp"
#include <cg/tool_box.hpp>
#include <cg/canvas_itens/line.hpp>

namespace cg {
    LineTool::LineTool(ToolBox& tool_box) : Painter{ tool_box }, ghostLine{ toolBox.canvas } {
        ghostLine.getFrom = [this]() { return getGhostLineStart(); };
        ghostLine.getTo = [this]() { return getGhostLineEnd(); };
    }

    void LineTool::_onRender() {
        ghostLine._render();
    }

    void LineTool::_input(io::MouseMove mouse_event) {
        if (isDrawing)
            setPosition(mouse_event.position);
    }

    void LineTool::_input(io::MouseDrag mouse_event) {
        if (isDrawing)
            setPosition(mouse_event.position);
    }

    void LineTool::_input(io::MouseLeftButtonPressed mouse_event) {
        setPosition(mouse_event.position);

        if (isDrawing) {
            line->append(mouse_event.position);
        }
        else {
            // Use cor global para novo objeto
            line = new Line(mouse_event.position, toolBox.currentColor);
            appendToCanvas(line);

            // NÃO vincular ponteiro da cor do objeto para toolBox aqui
            // Vincular seleção para edição da cor
            toolBox.getSelectorTool().select(line);

            isDrawing = true;
        }
    }

    void LineTool::_input(io::MouseRightButtonPressed mouse_event) {
        isDrawing = false;

        if (line != nullptr && line->size() < 2)
            delete line;

        line = nullptr;
        toolBox.unbindColorPtr(); // desativa vinculação
    }

    Vector2 LineTool::getGhostLineStart() const {
        assert_err(line != nullptr, "No line");
        return line->lastVertice();
    }

    Vector2 LineTool::getGhostLineEnd() const {
        return getPosition();
    }
}
