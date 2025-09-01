#include "ghost_line.hpp"

#include <cg/canvas.hpp>
#include <cg/math.hpp>


namespace cg {

    GhostLine::GhostLine(Canvas* canvas) : canvas{ canvas } {}

    GhostLine::GhostLine(std::function<Vector2()> getFrom, std::function<Vector2()> getTo, Canvas* canvas)
        : getFrom{ getFrom }, getTo{ getTo }, canvas{ canvas } {
    }

    GhostLine::GhostLine(std::function<Vector2()> getFrom, std::function<Vector2()> getTo, Color color, Canvas* canvas)
        : getFrom{ getFrom }, getTo{ getTo }, color{ color }, canvas{ canvas } {}

    void GhostLine::_render() {
        assert_err(getFrom != nullptr, "`getFrom` is `nullptr` on render");
        assert_err(getTo != nullptr, "`getTo` is `nullptr` on render");

        Vector2 from = getFrom();
        Vector2 to = getTo();

        if (from == to)
            return;

        GLdebug() {
            const float SEGMENT_LENGTH = DASH_LENGTH + GAP_LENGTH;

            float screenDistance = from.distance(to);
            if (screenDistance < 1e-5f)
                return; // avoid low precision issues

            Vec2Interpolator interpolator(from, to, DASH_LENGTH);

            glBegin(GL_LINES);
            glColor4f(color.r, color.g, color.b, color.a);

            bool drawSegment = true;
            Vector2 prev = from;
            int segmentCount = 0;

            for (Vector2 current : interpolator) {

                // Alterna entre tra�o e espa�o a cada DASH_LENGTH
                if (drawSegment) {
                    Vector2 ndcPrev = prev;
                    Vector2 ndcCurrent = current;

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

}