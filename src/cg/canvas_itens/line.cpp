#include "line.hpp"

namespace cg {

	void Line::_render()
	{
		if (vertices.empty())
			return; // A linha deve ter pelo menos 2 vértices (a posição do item conta como 1 vértice)

		// Faremos manualmente!
		// aplica transformação do modelo com open gl
		//glMatrixMode(GL_MODELVIEW);

		GLdebug{
			glLineWidth(width);
		}
		GLdebug{
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

	std::ostream& Line::_print(std::ostream& os) const
	{
		os << "Line: " << model << ", width: " << width << ", color: " << color << ", vertices[";

		auto vertice = vertices.begin();
		if (vertice < vertices.end()) {
			os << *vertice;
			for (vertice = ++vertice; vertice < vertices.end(); ++vertice)
				os << ", " << *vertice;
		}
		return os << ']';
	}

	std::ofstream& Line::_serialize(std::ofstream& ofs) const
	{
		ofs << model << " width: " << width << " color: " << color << " vertices[";

		auto vertice = vertices.begin();
		if (vertice < vertices.end()) {
			ofs << *vertice;
			for (vertice = ++vertice; vertice < vertices.end(); ++vertice)
				ofs << ", " << *vertice;
		}
		else
			ofs << ' '; // se não houver vértices, imprime separador vazio
		ofs << ']';
		return ofs;
	}

	std::ifstream& Line::_deserialize(std::ifstream& ifs)
	{
		try {
			std::string dummy;
			if (!(ifs >> model >> dummy >> width >> dummy >> color >> dummy))
				ifs.setstate(std::ios::failbit); // marca falha no stream

			vertices.clear();
			// Lê os vértices

			while (std::isspace(ifs.peek()))
				ifs.ignore();
			if (ifs.peek() == ']') {
				ifs.ignore();
				return ifs; // não há vértices
			}

			Vector2 vertice;
			while (ifs >> vertice) {
				vertices.push_back(vertice);
				while (std::isspace(ifs.peek()))
					ifs.ignore(); // ignora espaços em branco
				if (ifs.peek() == ',')
					ifs.ignore(); // ignora vírgula
				else if (ifs.peek() == ']')
					break; // fim da lista de vértices
			}
		}
		catch (...) {
			ifs.setstate(std::ios::failbit);
		}
		return ifs;
	}

}
