#include <memory>
#include <util.hpp>
#include <math.h>

#include "polygon_tool.hpp"

#include <cg/tool_box.hpp>
#include <cg/canvas_itens/polygon.hpp>

namespace cg {

    PolygonTool::PolygonTool(ToolBox& tool_box)
        : Painter{ tool_box }, lines{ GhostLine{ toolBox.canvas }, GhostLine{ toolBox.canvas } }
    {
        lines[0].getFrom = [this]() { return getFirstGhostLineStart(); };
        lines[1].getFrom = [this]() { return getLastGhostLineStart(); };

        for (auto i : { 0, 1 })
            lines[i].getTo = [this]() { return getGhostLineEnd(); };

        lines[0].color.r = 1.0f - lines[0].color.r;
    }

    void PolygonTool::_onRender() {
        for (auto& line : lines)
            line._render();
    }

    void PolygonTool::_input(io::MouseMove mouse_event) {
        if (isDrawing)
            setPosition(mouse_event.position);
    }

    void PolygonTool::_input(io::MouseDrag mouse_event) {
        if (isDrawing)
            setPosition(mouse_event.position);
    }

    void PolygonTool::_input(io::MouseLeftButtonPressed mouse_event) {
        if (isDrawing) {
            polygon->append(getPosition());
        }
        else {
            polygon = new Polygon(mouse_event.position, toolBox.currentColor);
            appendToCanvas(polygon);

            // NÃO vincular ponteiro da cor do objeto
            toolBox.getSelectorTool().select(polygon);

            setPosition(mouse_event.position);
            isDrawing = true;
        }
    }

    void PolygonTool::_input(io::MouseRightButtonPressed mouse_event) {
        isDrawing = false;
        polygon = nullptr;

        toolBox.unbindColorPtr();
    }

    Vector2 PolygonTool::getFirstGhostLineStart() const {
        assert_err(polygon != nullptr, "No polygon");
        return polygon->firstVertice();
    }

    Vector2 PolygonTool::getLastGhostLineStart() const {
        assert_err(polygon != nullptr, "No polygon");
        return polygon->lastVertice();
    }

    Vector2 PolygonTool::getGhostLineEnd() const {
        return getPosition();
    }
}
