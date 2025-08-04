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

    void PointTool::_render()
    {
        if (isDrawing)
            return;
        GLdebug{
            glPointSize(Point::SIZE + 1.0f);
            glColor3ub(255, 255, 255);
        }
        GLdebug {
        glBegin(GL_POINTS);
            glVertex2f(position.x, position.y);
        glEnd();
        }
        GLdebug{
            glPointSize(Point::SIZE);
            glColor3ub(0, 0, 0);
        }
        GLdebug{
        glBegin(GL_POINTS);
            glVertex2f(position.x, position.y);
        glEnd();
        }
    }

    void PointTool::_input(io::MouseMove mouse_event)
    {
        position = mouse_event.position;
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
        position = mouse_event.position;
        isDrawing = false; // Não mova mais o último ponto.
    }
}