#include <memory>

#include "point_tool.hpp"

#include <cg/canvas_itens/point.hpp>


namespace cg {

    PointTool::PointTool(ToolBox& tool_box) : Painter{ tool_box } {}

    void PointTool::_render()
    {
        // TODO -> Verificar se está desenhando acima do ponto
        if (toolBox.isInsideGui || isDrawing)
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
        }
    }

    void cg::PointTool::_input(io::MouseLeftButtonPressed mouse_event)
    {
        // New point primitive
        point = new Point(mouse_event.position, toolBox.getColor());
        toolBox.bindColorPtr(&point->getColor());
        appendToCanvas(point);

        isDrawing = true;
    }

    void cg::PointTool::_input(io::MouseLeftButtonReleased mouse_event)
    {
        position = mouse_event.position;
        isDrawing = false; // Não mova mais o último ponto.
    }
}