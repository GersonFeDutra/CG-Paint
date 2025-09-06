#include <cg/canvas_itens/point.hpp>
#include <cg/canvas_itens/line.hpp>
#include <cg/canvas_itens/polygon.hpp>

#include "select_tool.hpp"


namespace cg {

    void SelectTool::_input(io::MouseMove mouse_event) {
        if (auto* item = toolBox.canvas->pick(mouse_event.position); item != nullptr)
            glutSetCursor(GLUT_CURSOR_INFO);
        else
            glutSetCursor(GLUT_CURSOR_INHERIT);
    }

    void SelectTool::select(CanvasItem *item) {
        selectedItem = item;
        // A transformação do item selecionado é uma cópia da seleção.
        // Usamos isso para fazer transformações absolutas na ferramenta de seleção,
        // e relativas no ítem selecionado com deltas.
        model = item->model;
        gizmo.attach(&item->model);

        switch (item->typeInfo) {
            case CanvasItem::TypeInfo::POINT:
                toolBox.bindColorPtr(&((Point *)(item))->getColor());
                break;
            case CanvasItem::TypeInfo::LINE:
                toolBox.bindColorPtr(&((Line *)(item))->getColor());
                break;
            case CanvasItem::TypeInfo::POLYGON:
                toolBox.bindColorPtr(&((Polygon *)(item))->getColor());
                break;
            default:
                break;
        }
    }
}
