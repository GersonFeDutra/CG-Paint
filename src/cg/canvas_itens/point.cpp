#include "point.hpp"
#include <cstdlib>

#include <util.hpp>


namespace cg
{
    Point::Point(Vector2 position, Color color) : CanvasItem{ TypeInfo::POINT, position }, color{ color } {
        SET_CLI_BLUE();
        printf("%f, %f\n", position.x, position.y);
        RESET_CLI();
    }

    // Sele��o de ponto
    // Verifica se a posi��o do mouse est� pr�xima de um ponto.
    // Retorna true se a dist�ncia entre mousePos e point for menor ou igual ao threshold.
    bool Point::isSelected(Vector2 mousePos) const
    {
        return (mousePos - position).length() <= size + CanvasItem::SELECTION_THRESHOLD;
    }

    void Point::_render()
    {
        GLdebug{
            glPointSize(SIZE);
        }
        GLdebug{
            glBegin(GL_POINTS);
                glColor3f(color.r, color.g, color.b);
                glVertex2f(position.x, position.y);
            glEnd();
        }
    }

    void Point::_input(io::MouseDrag mouse_event)
    {
        position = mouse_event.position;
    }

    std::ostream& Point::_print(std::ostream& os) const
    {
        return os << "Point: " << position << ", size: " << size << ", color: " << color;
    }

    std::ofstream& Point::_serialize(std::ofstream& fs) const
    {
        fs << position << " size: " << size << " color: " << color;
        return fs;
    }

    std::ifstream& Point::_deserialize(std::ifstream& fs)
    {
        try {
            std::string dummy;
            if (!(fs >> position >> dummy >> size >> dummy >> color))
                fs.setstate(std::ios::failbit); // marca falha no stream
        }
        catch (...) {
            fs.setstate(std::ios::failbit);
        }
        return fs;
    }

    Points::Points(Color point_color, ArrayList<Vector2> point_list) : pointColor(point_color), pointList(point_list) {}

    void Points::_render()
    {
        GLdebug {
            glColor3f(pointColor.r, pointColor.g, pointColor.b);
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
