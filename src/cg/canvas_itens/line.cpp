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

	// Retorna true se o segmento p1-p2 intercepta o retângulo centrado em mousePos com tamanho threshold
	static inline bool lineSelectedRect(Vector2 mousePos,
			Vector2 p1, Vector2 p2,
			float threshold = CanvasItem::SELECTION_THRESHOLD)
	{
		// Definir os limites do retângulo de tolerância
		float left   = mousePos.x - threshold;
		float right  = mousePos.x + threshold;
		float top    = mousePos.y - threshold;
		float bottom = mousePos.y + threshold;

		// Codificação dos pontos usando 4 bits (Cohen–Sutherland)
		auto computeCode = [&](Vector2 v) -> int {
			int code = 0;
			if (v.x < left)   code |= 1; // Esquerda
			if (v.x > right)  code |= 2; // Direita
			if (v.y < top)    code |= 4; // Cima
			if (v.y > bottom) code |= 8; // Baixo
			return code;
		};

		int code1 = computeCode(p1);
		int code2 = computeCode(p2);

		while (true) {
			if ((code1 | code2) == 0) {
				// Ambos dentro do retângulo → intercepta
				return true;
			} else if ((code1 & code2) != 0) {
				// Ambos fora da mesma região → não intercepta
				return false;
			} else {
				// Pelo menos um ponto está fora → calcula interseção
				int outCode = code1 ? code1 : code2;
				Vector2 newPoint;

				if (outCode & 8) {            // baixo
					newPoint.x = p1.x + (p2.x - p1.x) * (bottom - p1.y) / (p2.y - p1.y);
					newPoint.y = bottom;
				} else if (outCode & 4) {     // cima
					newPoint.x = p1.x + (p2.x - p1.x) * (top - p1.y) / (p2.y - p1.y);
					newPoint.y = top;
				} else if (outCode & 2) {     // direita
					newPoint.y = p1.y + (p2.y - p1.y) * (right - p1.x) / (p2.x - p1.x);
					newPoint.x = right;
				} else if (outCode & 1) {     // esquerda
					newPoint.y = p1.y + (p2.y - p1.y) * (left - p1.x) / (p2.x - p1.x);
					newPoint.x = left;
				}

				// Substituir ponto externo pelo ponto de interseção
				if (outCode == code1) {
					p1 = newPoint;
					code1 = computeCode(p1);
				} else {
					p2 = newPoint;
					code2 = computeCode(p2);
				}
			}
		}
	}

	bool Line::_isSelected(Vector2 cursor_local_position) const
	{
		if (vertices.size() < 2)
			return false;

		auto iterator = vertices.begin();
		Vector2 lastVertice = *iterator;
		for (++iterator; iterator != vertices.end(); ++iterator) {
			Vector2 vertice = *iterator;
			if (lineSelectedRect(
					cursor_local_position, lastVertice, vertice,
					CanvasItem::SELECTION_THRESHOLD + width))
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
