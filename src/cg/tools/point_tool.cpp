#include "point_tool.hpp"
#include "point_tool.hpp"
#include "point_tool.hpp"
#include <memory>

#include "point_tool.hpp"

#include <cg/canvas_itens/point.hpp>


namespace cg {

    PointTool::PointTool(ToolBox& tool_box) : Painter{ tool_box } {}

    CanvasItem* PointTool::make(Vector2 at) {
        // New point primitive
        ColorRgb color = Color(toolBox.currentColor);
        auto point = std::make_unique<Point>(at, color);
        CanvasItem* ptr = point.get();
        toolBox.canvas->insert(std::move(point));
        return ptr;
    }

    void PointTool::_input(io::MouseDrag mouse_event)
    {
        if (isDrawing) {
            assert_err(point != nullptr, "Drawing a not intanced primitive.");
            point->position = mouse_event.position;
            // REFACTOR -> Note que a posição dessa ferramenta nunca é alterada.
            // Poderia ser útil sincronizar a posição do ponto com a posição da ferramenta?
        }
    }

    void cg::PointTool::_input(io::MouseLeftButtonPressed mouse_event)
    {
        point = (Point *)make(mouse_event.position);
        isDrawing = true;
    }

    void cg::PointTool::_input(io::MouseLeftButtonReleased mouse_event)
    {
        isDrawing = false; // Não mova mais o último ponto.
    }
}