#include <cstdlib>

#include "util.hpp"

#include "point.hpp"

namespace cg
{
    Point::Point(Vector2 position, ColorRgb color) : position(position), color(color) {
        SET_CLI_YELLOW();
        printf("%f, %f\n", position.x, position.y);
        RESET_CLI();
    }

    void Point::_render()
    {
        GLdebug{
            glPointSize(10.0f);
        }
        GLdebug{
            glBegin(GL_POINTS);
                glColor3ub(color.r, color.g, color.b);
                glVertex2f(position.x, position.y);
            glEnd();
        }
    }

    Points::Points(ColorRgb point_color, ArrayList<Vector2> point_list) : pointColor(point_color), pointList(point_list) {}

    void Points::_render()
    {
        GLdebug {
            glColor3ub(pointColor.r, pointColor.g, pointColor.b);
        }
        GLdebug {
            glBegin(GL_POINTS);
                for (Vector2 point : pointList) {
                    glVertex2i(point.x, point.y);
                }
            glEnd();
        }
    }


} // namespace cg
