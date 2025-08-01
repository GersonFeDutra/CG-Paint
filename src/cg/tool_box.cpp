#include "tool_box.hpp"

#include "canvas.hpp"

#include "canvas_itens/point.hpp"


namespace cg {

    void ToolBox::addCanvas(Canvas* canvas_ptr) {
        assert_err(canvas == nullptr, "Canvas already added.");
        canvas = canvas_ptr;
    }

    CanvasItem* ToolBox::newPrimitive(Vector2 at) {
        assert_err(canvas != nullptr, "Canvas was not added.");

        switch (currentPrimitive)
        {
        case POINT: {
            // new point primitive
            ColorRgb color = cg::Color(currentColor);
            auto point = std::make_unique<Point>(at, color);
            CanvasItem* ptr = point.get();
            canvas->insert(std::move(point));
            return ptr;
        } break;
        case LINE: {
            // TODO
            //canvas.insert(std::make_unique<cg::Line>(from, to, color));
            return nullptr;
        } break;
        case POLYGON: {
            // TODO
            //canvas.insert(std::make_unique<cg::Polygon>(radius, edges, color));
            return nullptr;
        } break;
        default:
            return nullptr;   
        }
    }
}