#include "point.hpp"
#include <cstdlib>

#include <util.hpp>


namespace cg
{
    Point::Point(Vector2 position, Color color) : CanvasItem{ TypeInfo::POINT, position }, color{ color } {}

    // Seleção de ponto
    // Verifica se a posição do mouse está próxima de um ponto.
    // Retorna true se a distância entre mousePos e point for menor ou igual ao threshold.
    static inline bool point_selected(Vector2 local_position, Vector2 point, float threshold = CanvasItem::SELECTION_THRESHOLD) {
        return (local_position - point).length() <= threshold;
	}

    bool Point::_isSelected(Vector2 cursor_local_position) const
    {
		return point_selected(cursor_local_position, getPosition(), size + CanvasItem::SELECTION_THRESHOLD);
    }

    void Point::_render()
    {
        GLdebug{
            glPointSize(size);
        }
        GLdebug{
            glBegin(GL_POINTS);
                glColor3f(color.r, color.g, color.b);
				auto [x, y] = getPosition();
                glVertex2f(x, y);
            glEnd();
        }
    }

    void Point::_input(io::MouseDrag mouse_event)
    {
        model = Transform2D(mouse_event.position);
    }

    std::ostream& Point::_print(std::ostream& os) const
    {
        return os << "Point: " << getPosition() << ", size: " << size << ", color: " << color;
    }

    std::ofstream& Point::_serialize(std::ofstream& fs) const
    {
        fs << getPosition() << " size: " << size << " color: " << color;
        return fs;
    }

    std::ifstream& Point::_deserialize(std::ifstream& fs)
    {
        try {
            std::string dummy;
            Vector2 newPosition;
            Color newColor;
            float newSize;
            if (!(fs >> newPosition >> dummy >> newSize) || dummy != "size:" || !(fs >> dummy >> newColor) || dummy != "color:") {
                fs.setstate(std::ios::failbit); // marca falha no stream
				return fs;
            }
			color = newColor;
			setPosition(newPosition);
        }
        catch (...) {
            fs.setstate(std::ios::failbit);
        }
        return fs;
    }


} // namespace cg
