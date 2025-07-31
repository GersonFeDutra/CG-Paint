#include <cstdlib>

#include "util.hpp"

#include "point.hpp"

namespace cg
{
    int Point::init(ColorRgb pointColor, ArrayList<int> coords) {
        GLdebug {
            glColor3ub(pointColor.r, pointColor.g, pointColor.b);
        }
        GLdebug {
            glBegin(GL_POINTS);
                glVertex2i(coords[0], coords[1]);
            glEnd();
        }
        return EXIT_SUCCESS;
    }
} // namespace cg
