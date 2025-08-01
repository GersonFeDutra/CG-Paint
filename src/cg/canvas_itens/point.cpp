#include <cstdlib>

#include "util.hpp"

#include "point.hpp"

namespace cg
{
    Points::Points(ColorRgb point_color, ArrayList<int> coords) : pointColor(point_color), coords(coords) {}

    void Points::_render()
    {
        GLdebug {
            glColor3ub(pointColor.r, pointColor.g, pointColor.b);
        }
        GLdebug {
            glBegin(GL_POINTS);
                glVertex2i(coords[0], coords[1]);
            glEnd();
        }
    }
} // namespace cg
