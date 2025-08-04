#include <memory>

#include <util.hpp>
#include <math.h>

#include "line_tool.hpp"

#include <cg/tool_box.hpp>
#include <cg/canvas_itens/line.hpp>


namespace cg {

    CanvasItem* LineTool::make(Vector2 at) {
        assert_err(toolBox.canvas != nullptr, "Canvas was not added.");

        // New line primitive
        ColorRgb color = Color(toolBox.currentColor);
        auto line = std::make_unique<Line>(at, color);
        CanvasItem* ptr = line.get();
        toolBox.canvas->insert(std::move(line));
        return ptr;
	}

    // TODO -> move iterator to math.hpp for later use.
    // Gera uma interpolação de from à to usando iteradores.
    class Vec2Interpolator {
    public:
        Vec2Interpolator(Vector2 from, Vector2 to, float step_size)
            : from(from), to(to), step_size(step_size)
        {
            total_distance = from.distance(to);
            steps = static_cast<int>(total_distance / step_size) + 1;
        }

        class Iterator {
        public:
            Iterator(Vec2Interpolator* interpolator, int step)
                : interpolator(interpolator), step(step) {
            }

            Vector2 operator*() const {
                float t = static_cast<float>(step) / interpolator->steps;
                return interpolator->from.lerp(interpolator->to, t);
            }

            Iterator& operator++() { step++; return *this; }
            // Verifica se a direção atual entre os pontos corresponde a direção alvo.
            // Se o sinal do produto escalar das direções mudar, significa que elas mudaram de direção (passou do ponto alvo).
            bool operator!=(const Iterator& other) const { return step != other.step; }

        private:
            Vec2Interpolator* interpolator;
            int step;
        };

        Iterator begin() { return Iterator(this, 0); }
        Iterator end() { return Iterator(this, steps + 1); } // +1 para incluir o ponto final

    private:
        Vector2 from;
        Vector2 to;
        float step_size;
        float total_distance;
        int steps;
    };

    // FIXME
    void LineTool::_render() {
        if (!isDrawing)
            return;

        // Draws dashed line
        GLdebug{
            int alt = 0;

            const float SEGMENT_LENGHT = 2.5f; // length of each segment onto the dashed line.
            Vector2 start = (line->size() == 0) ? line->position : line->frontVertice();
            // Convert to screen coordinates to avoid minor precision issues [too close to 0].
            start = toolBox.canvas->ndcToScreen(start);
            Vector2 end = toolBox.canvas->ndcToScreen(position);

            Vec2Interpolator interpolator(start, end, SEGMENT_LENGHT);
            auto it = interpolator.begin();

            glBegin(GL_LINES);
            glColor4f(colors::WHITE.r, colors::WHITE.g, colors::WHITE.b, colors::WHITE.a);

            for (++it; it != interpolator.end(); ++it) {
                Vector2 current = toolBox.canvas->screenToNdc(*it);

                if (alt == 1 || alt == 2)
                    glVertex2f(current.x, current.y);
                alt++;
                alt %= 3;
            }
            glEnd();
        }
    }

    void LineTool::_input(io::MouseMove mouse_event)
    {
        if (isDrawing)
            position = mouse_event.position;
    }


    void LineTool::_input(io::MouseLeftButtonPressed mouse_event)
    {
        if (isDrawing)
            line->append(mouse_event.position);
        else {
            line = (Line *)make(mouse_event.position);
            isDrawing = true;
        }
    }

}