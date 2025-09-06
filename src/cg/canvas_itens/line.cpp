#include "line.hpp"

namespace cg {

	void Line::_render()
	{
		if (vertices.empty())
			return; // A linha deve ter pelo menos 2 vértices (a posição do item conta como 1 vértice)

		// Faremos manualmente!
		// aplica transformação do modelo com open gl
		//glMatrixMode(GL_MODELVIEW);

		GLdebug() {
			glLineWidth(width);
		}
		GLdebug() {
			glBegin(GL_LINE_STRIP);
				glColor3f(color.r, color.g, color.b);
				for (const auto& vertice : vertices) {
					auto [x, y] = model * vertice; // aplica a transformação do modelo em cada vértice
					glVertex2f(x, y);
				}
			glEnd();
		}

		//glLoadIdentity(); // Restaura a matriz de modelo
	}

	// Seleção de linha
	// Verifica se a posição do mouse está próxima de um segmento de reta definido por p1 e p2.
	// A proximidade é definida pelo threshold.
	static inline bool lineSelected(Vector2 mousePos, Vector2 p1, Vector2 p2, float threshold = CanvasItem::SELECTION_THRESHOLD) {
		Vector2 line = p2 - p1;
		Vector2 toMouse = mousePos - p1;
		float lineLength = line.length();

		//Se linha for muito pequena, ignora o seguimento
		if (lineLength < 1e-5f) return false;

		// Projeção do mouse na linha normalizada em relação ao segmento
		// O Clamp entre 0 e 1 garante que o ponto projetado esteja dentro do segmento
		float t = std::max(0.0f, std::min(1.0f, toMouse.dot(line) / (lineLength * lineLength)));
		Vector2 closest = p1 + line * t;

		return (mousePos - closest).length() <= threshold;
	}

	bool Line::_isSelected(Vector2 cursor_local_position) const
	{
		// TODO -> Fazer seleção de ponto no caso que size < 2
		if (vertices.size() < 2) // A linha deve ter pelo menos 2 vértices 
			return false;

		auto iterator = vertices.begin();
		Vector2 lastVertice = *iterator;
		for (++iterator; iterator < vertices.end(); ++iterator) {
			Vector2 vertice = *iterator;
			if (lineSelected(cursor_local_position, lastVertice, vertice, CanvasItem::SELECTION_THRESHOLD + width))
				return true;
			lastVertice = vertice;
		}
		return false;
	}

// 	std::ostream& Line::_print(std::ostream& os) const
// 	{
// 		os << "Line: " << model << ", width: " << width << ", color: " << color << ", vertices[";
// 
// 		auto vertice = vertices.begin();
// 		if (vertice < vertices.end()) {
// 			os << *vertice;
// 			for (vertice = ++vertice; vertice < vertices.end(); ++vertice)
// 				os << ", " << *vertice;
// 		}
// 		return os << ']';
// 	}

	std::ostream& Line::_serialize(std::ostream& os) const
	{
		os << "Line " << model << " width: " << width << " color: " << color << " vertices[ ";

		auto vertice = vertices.begin();
		if (vertice < vertices.end()) {
			os << *vertice;
			for (vertice = ++vertice; vertice < vertices.end(); ++vertice)
				os << ' ' << *vertice;
		}
		os << " ]";
		return os;
	}

	std::istream& Line::_deserialize(std::istream& is)
	{
		try {
			std::string dummy;
			float newWidth;
			Color newColor;
			Transform2D newModel;
			ArrayList<Vector2> newVertices;

			if constexpr (IS_DEBUG) {
				std::string dummy;

				// 1. Lê "Line" + model
				if (!(is >> dummy >> newModel)) {
					print_error("Falha ao ler 'Line <model>'");
					is.setstate(std::ios::failbit);
					return is;
				}
				if (dummy != "Line") {
					print_error("Esperado 'Line', mas veio '%s'", dummy.c_str());
					is.setstate(std::ios::failbit);
					return is;
				}

				// 2. Lê "width:" + width
				if (!(is >> dummy >> newWidth)) {
					print_error("Falha ao ler 'width: <valor>'");
					is.setstate(std::ios::failbit);
					return is;
				}
				if (dummy != "width:") {
					print_error("Esperado 'width:', mas veio '%s'", dummy.c_str());
					is.setstate(std::ios::failbit);
					return is;
				}

				// 3. Lê "color:" + color
				if (!(is >> dummy >> newColor)) {
					print_error("Falha ao ler 'color: <valor>'");
					is.setstate(std::ios::failbit);
					return is;
				}
				if (dummy != "color:") {
					print_error("Esperado 'color:', mas veio '%s'", dummy.c_str());
					is.setstate(std::ios::failbit);
					return is;
				}

				// 4. Lê "vertices["
				if (!(is >> dummy)) {
					print_error("Falha ao ler 'vertices['");
					is.setstate(std::ios::failbit);
					return is;
				}
				if (dummy != "vertices[") {
					print_error("Esperado 'vertices[', mas veio '%s'", dummy.c_str());
					is.setstate(std::ios::failbit);
					return is;
				}
			}
			else {
				if (!(is >> dummy >> newModel) || dummy != "Line" ||
					!(is >> dummy >> newWidth) || dummy != "width:" ||
					!(is >> dummy >> newColor) || dummy != "color:" ||
					!(is >> dummy) || dummy != "vertices[")
				{
					is.setstate(std::ios::failbit); // marca falha no stream
				}
			}
			// Lê os vértices
			Vector2 vertice;
			while (is_next_vec2<float>(is)) {
				is >> vertice;
				newVertices.push_back(vertice);
			}
			if (!(is >> dummy) || dummy != "]")
				is.setstate(std::ios::failbit); // marca falha no stream
			else
				is.clear(); // limpa possíveis flags

			model = newModel;
			width = newWidth;
			color = newColor;
			vertices = newVertices;
		}
		catch (...) {
			is.setstate(std::ios::failbit);
		}
		return is;
	}

}
