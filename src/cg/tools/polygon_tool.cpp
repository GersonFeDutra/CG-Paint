#include <memory>

#include <util.hpp>
#include <math.h>

#include "polygon_tool.hpp"

#include <cg/tool_box.hpp>
#include <cg/canvas_itens/polygon.hpp>


namespace cg {

    CanvasItem* PolygonTool::make(Vector2 at) {
        assert_err(toolBox.canvas != nullptr, "Canvas was not added.");

        // New line primitive
        ColorRgb color = Color(toolBox.currentColor);
        auto line = std::make_unique<Line>(at, color);
        CanvasItem* ptr = line.get();
        toolBox.canvas->insert(std::move(line));

        std::cout << "Made polygon temporary line" << std::endl;
        return ptr;
	}

    CanvasItem* PolygonTool::makePolygon(Vector2 at) {
        assert_err(toolBox.canvas != nullptr, "Canvas was not added.");

        // New polygon primitive
        ColorRgb color = Color(toolBox.currentColor);
        auto poly = std::make_unique<Polygon>(at, color);
        CanvasItem* ptr = poly.get();
        toolBox.canvas->insert(std::move(poly));

        std::cout << "Made new polygon" << std::endl;
        return ptr;
	}

    class Vec2Interpolator {
    public:
        Vec2Interpolator(Vector2 from, Vector2 to, float step_size)
            : from(from), to(to), step_size(step_size)
        {
            total_distance = from.distance(to);

            if (total_distance < 1e-5f) {
                steps = 1;
            }
            else {
                steps = std::max(2, static_cast<int>(std::ceil(total_distance / step_size)));
            }
        }

        class Iterator {
        public:
            Iterator(Vec2Interpolator* interpolator, int step)
                : interpolator(interpolator), step(step) {
            }

            Vector2 operator*() const {
                float t = (interpolator->steps > 1)
                    ? static_cast<float>(step) / (interpolator->steps - 1)
                    : 0.0f;
                return interpolator->from.lerp(interpolator->to, t);
            }

            Iterator& operator++() {
                step++;
                return *this;
            }

            bool operator!=(const Iterator& other) const {
                return step != other.step;
            }

        private:
            Vec2Interpolator* interpolator;
            int step;
        };

        Iterator begin() { return Iterator(this, 0); }
        Iterator end() { return Iterator(this, steps); }
        int getSteps() const { return steps; }

    private:
        Vector2 from;
        Vector2 to;
        float step_size;
        float total_distance;
        int steps;
    };

    void PolygonTool::_render() {
        if (!isDrawing || toolBox.isInsideGui)
            return;

        GLdebug{
            const float SEGMENT_LENGTH = DASH_LENGTH + GAP_LENGTH;

            Vector2 start = tempLine->lastVertice();
            Vector2 screenStart = toolBox.canvas->ndcToScreen(start);
            Vector2 screenEnd = toolBox.canvas->ndcToScreen(position);

            float screenDistance = screenStart.distance(screenEnd);
            if (screenDistance < 1e-5f)
                return; // avoid low precision issues

            Vec2Interpolator interpolator(screenStart, screenEnd, DASH_LENGTH);

            glBegin(GL_LINES);
            glColor4f(GHOST_LINE_COLOR.r, GHOST_LINE_COLOR.g, GHOST_LINE_COLOR.b, GHOST_LINE_COLOR.a);

            bool drawSegment = true;
            Vector2 prev = screenStart;
            int segmentCount = 0;

            for (auto it = interpolator.begin(); it != interpolator.end(); ++it) {
                Vector2 current = *it;

                // Alterna entre tra�o e espa�o a cada DASH_LENGTH
                if (drawSegment) {
                    Vector2 ndcPrev = toolBox.canvas->screenToNdc(prev);
                    Vector2 ndcCurrent = toolBox.canvas->screenToNdc(current);

                    glVertex2f(ndcPrev.x, ndcPrev.y);
                    glVertex2f(ndcCurrent.x, ndcCurrent.y);
                }

                // A cada DASH_LENGTH percorrida, alterna o estado
                segmentCount++;
                if (segmentCount % 2 == 0) {
                    drawSegment = !drawSegment;
                }

                prev = current;
            }

            glEnd();
        }
    }


    void PolygonTool::_input(io::MouseMove mouse_event)
    {
        if (isDrawing)
            position = mouse_event.position;
    }

    void PolygonTool::_input(io::MouseDrag mouse_event)
    {
        if (isDrawing)
            position = mouse_event.position;
    }


    void PolygonTool::_input(io::MouseLeftButtonPressed mouse_event)
    {
        if (isDrawing)
            tempLine->append(mouse_event.position);
        else {
            tempLine = (Line *)make(mouse_event.position);
            isDrawing = true;
            position = mouse_event.position;
            initialCoords = Vector2(position.x, position.y);
        }
    }

    void PolygonTool::_input(io::MouseRightButtonPressed mouse_event)
    {
        polygons.push_back((Polygon *) makePolygon(initialCoords));
        polygons.back()->setVertices(tempLine->getVertices());

        isDrawing = false;

        delete tempLine;
        tempLine = nullptr;
    }

}