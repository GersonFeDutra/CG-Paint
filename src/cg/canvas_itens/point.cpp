#include <string>
#include "point.hpp"
#include <cstdlib>

#include <util.hpp>


namespace cg
{
    Point::Point(Vector2 position, Color color) : CanvasItem{ TypeInfo::POINT }, color{ color }, localPosition{ position } {}

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
        GLdebug() {
            //glEnable(GL_POINT_SMOOTH);
            //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            glPointSize(size);
        }
        GLdebug() {
            glBegin(GL_POINTS);
                glColor3f(color.r, color.g, color.b);
				auto [x, y] = getPosition();
                glVertex2f(x, y);
            glEnd();
        }
    }

    // std::ostream& Point::_print(std::ostream& os) const
    // {
    //     return os << "Point: " << model << " at: " << getPosition() << ", size : " << size << ", color : " << color;
    // }

	std::ostream &Point::_serialize(std::ostream &os) const
    {
        os << "Point " << model << " at: " << getPosition() << " size: " << size << " color: " << color;
        return os;
    }

    std::istream& Point::_deserialize(std::istream& is)
    {
        try {
            std::string dummy;
            Vector2 newPosition;
            Color newColor;
            float newSize;
            Transf2x3<float> newModel;
            if constexpr (IS_DEBUG) {
                if (!(is >> dummy >> newModel)) {
                    print_error("Erro ao ler 'Point <newModel>'");
                    is.setstate(std::ios::failbit);
                    return is;
                }
                if (dummy != "Point") {
                    print_error("Esperado 'Point', mas veio: %s", dummy);
                    is.setstate(std::ios::failbit);
                    return is;
                }

                if (!(is >> dummy >> newPosition)) {
                    print_error("Erro ao ler 'at: <newPosition>'");
                    is.setstate(std::ios::failbit);
                    return is;
                }
                if (dummy != "at:") {
                    print_error("Esperado 'at:', mas veio: %s", dummy);
                    is.setstate(std::ios::failbit);
                    return is;
                }

                if (!(is >> dummy >> newSize)) {
                    print_error("Erro ao ler 'size: <newSize>'");
                    is.setstate(std::ios::failbit);
                    return is;
                }
                if (dummy != "size:") {
                    print_error("Esperado 'size:', mas veio: %s", dummy);
                    is.setstate(std::ios::failbit);
                    return is;
                }

                if (!(is >> dummy >> newColor)) {
                    print_error("Erro ao ler 'color: <newColor>'");
                    is.setstate(std::ios::failbit);
                    return is;
                }
                if (dummy != "color:") {
                    print_error("Esperado 'color:', mas veio: %s", dummy);
                    is.setstate(std::ios::failbit);
                    return is;
                }
            }
            else {
                if (!(is >> dummy >> newModel) || dummy != "Point" ||
                    !(is >> dummy >> newPosition) || dummy != "at:" ||
                    !(is >> dummy >> newSize) || dummy != "size:" ||
                    !(is >> dummy >> newColor) || dummy != "color:") {
                    is.setstate(std::ios::failbit); // marca falha no stream
                    return is;
                }
            }
			color = newColor;
            size = newSize;
			setPosition(newPosition);
            model = newModel;
        }
        catch (...) {
            is.setstate(std::ios::failbit);
        }
        return is;
    }


} // namespace cg
