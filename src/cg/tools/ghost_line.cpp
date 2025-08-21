#include "ghost_line.hpp"
#include "ghost_line.hpp"


#include <cg/canvas.hpp>


namespace cg {
    GhostLine::GhostLine(Canvas* canvas) : canvas{ canvas } {}

    GhostLine::GhostLine(Vector2* from, Vector2* to, Canvas* canvas) : from{ from }, to{ to }, canvas{ canvas } {}

    GhostLine::GhostLine(Vector2* from, Vector2* to, Color color, Canvas* canvas) : from{ from }, to{ to }, color{ color }, canvas{ canvas } {}

    void GhostLine::_render() {
        assert_err(from != nullptr, "`from` is `nullptr` on render");
        assert_err(to != nullptr, "`to` is `nullptr` on render");

        if (from == to || *from == *to)
            return;

        //GLdebug{
            const float SEGMENT_LENGTH = DASH_LENGTH + GAP_LENGTH;

            Vector2 screenStart = canvas->ndcToScreen(*from);
            Vector2 screenEnd = canvas->ndcToScreen(*to);

            float screenDistance = screenStart.distance(screenEnd);
            if (screenDistance < 1e-5f)
                return; // avoid low precision issues

            Vec2Interpolator interpolator(screenStart, screenEnd, DASH_LENGTH);

            glBegin(GL_LINES);
            glColor4f(color.r, color.g, color.b, color.a);

            bool drawSegment = true;
            Vector2 prev = screenStart;
            int segmentCount = 0;

            for (Vector2 current : interpolator) {

                // Alterna entre traço e espaço a cada DASH_LENGTH
                if (drawSegment) {
                    Vector2 ndcPrev = canvas->screenToNdc(prev);
                    Vector2 ndcCurrent = canvas->screenToNdc(current);

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
        //}
	}

}