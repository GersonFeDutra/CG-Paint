#include <memory>

#include <util.hpp>
#include <math.h>

#include "polygon_tool.hpp"

#include <cg/tool_box.hpp>
#include <cg/canvas_itens/polygon.hpp>


namespace cg {

    PolygonTool::PolygonTool(ToolBox& tool_box)
        : Painter{ tool_box },
		lines{ // Inicializa as linhas com `from` e `to` apontando para a posição atual
            GhostLine{ &getPosition(), &getPosition(), toolBox.canvas },
            GhostLine{ &getPosition(), &getPosition(), toolBox.canvas } }
    {
        lines[0].color.r = 1.0f - lines[0].color.r;
    }

    void PolygonTool::_onRender() {
        for (auto& line : lines)
            line._render();
    }

    void PolygonTool::_input(io::MouseMove mouse_event)
    {
        if (isDrawing)
            setPosition(mouse_event.position);
    }

    void PolygonTool::_input(io::MouseDrag mouse_event)
    {
        if (isDrawing)
            setPosition(mouse_event.position);
    }


    void PolygonTool::_input(io::MouseLeftButtonPressed mouse_event)
    {
        if (isDrawing) {
            polygon->append(getPosition());
            lines[1].from = &polygon->lastVertice();
        }
        else {
            // new Polygon primitive
            polygon = new Polygon(mouse_event.position, toolBox.getColor());
            appendToCanvas(polygon); // add to canvas even if just one vertice (shown as a point)

            toolBox.bindColorPtr(&polygon->getColor());
            isDrawing = true;
            
            setPosition(mouse_event.position); // update position to the first vertice
            for (auto& line : lines)
                line.from = &polygon->lastVertice();

            isDrawing = true;
        }
    }

    void PolygonTool::_input(io::MouseRightButtonPressed mouse_event)
    {
        isDrawing = false;
        polygon = nullptr;

        for (auto& line : lines)
            line.from = &getPosition();

        toolBox.unbindColorPtr();
    }

}