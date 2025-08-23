#include "line.hpp"

namespace cg {

	void Line::_render()
	{
		if (vertices.empty())
			return; // A linha deve ter pelo menos 2 v�rtices (a posi��o do item conta como 1 v�rtice)
		GLdebug{
			glBegin(GL_LINE_STRIP);
				glColor3f(color.r, color.g, color.b);
				glVertex2f(position.x, position.y);
				for (auto& vertice : vertices)
					glVertex2f(vertice.x, vertice.y);
			glEnd();
		}
	}

	// Sele��o de linha
	// Verifica se a posi��o do mouse est� pr�xima de um segmento de reta definido por p1 e p2.
	// A proximidade � definida pelo threshold.
	static inline bool lineSelected(Vector2 mousePos, Vector2 p1, Vector2 p2, float threshold = CanvasItem::SELECTION_THRESHOLD) {
		Vector2 line = p2 - p1;
		Vector2 toMouse = mousePos - p1;
		float lineLength = line.length();

		//Se linha for muito pequena, ignora o seguimento
		if (lineLength < 1e-5f) return false;

		// Proje��o do mouse na linha normalizada em rela��o ao segmento
		// O Clamp entre 0 e 1 garante que o ponto projetado esteja dentro do segmento
		float t = std::max(0.0f, std::min(1.0f, toMouse.dot(line) / (lineLength * lineLength)));
		Vector2 closest = p1 + line * t;

		return (mousePos - closest).length() <= threshold;
	}

	// Sele��o de linha
	// Verifica se a posi��o do mouse est� pr�xima de um segmento de reta definido por p1 e p2.
	// A proximidade � definida pelo threshold.
	bool Line::isSelected(Vector2 mousePos) const
	{
		if (vertices.empty())
			return false; // A linha deve ter pelo menos 2 v�rtices (a posi��o do item conta como 1 v�rtice)

		Vector2 lastVertice = position;
		for (auto vertice : vertices) {
			if (lineSelected(mousePos, lastVertice, vertice))
				return true;
		}
		return false;
	}

	std::ostream& Line::_print(std::ostream& os) const
	{
		os << "Line: " << position << ", width: " << width << ", color: " << color << ", vertices[";

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
		ofs << position << " width: " << width << " color: " << color << " vertices[";

		auto vertice = vertices.begin();
		if (vertice < vertices.end()) {
			ofs << *vertice;
			for (vertice = ++vertice; vertice < vertices.end(); ++vertice)
				ofs << ", " << *vertice;
		}
		else
			ofs << ' '; // se n�o houver v�rtices, imprime separador vazio
		ofs << ']';
		return ofs;
	}

	std::ifstream& Line::_deserialize(std::ifstream& ifs)
	{
		try {
			std::string dummy;
			if (!(ifs >> position >> dummy >> width >> dummy >> color >> dummy))
				ifs.setstate(std::ios::failbit); // marca falha no stream

			vertices.clear();
			// L� os v�rtices

			while (std::isspace(ifs.peek()))
				ifs.ignore();
			if (ifs.peek() == ']') {
				ifs.ignore();
				return ifs; // n�o h� v�rtices
			}

			Vector2 vertice;
			while (ifs >> vertice) {
				vertices.push_back(vertice);
				while (std::isspace(ifs.peek()))
					ifs.ignore(); // ignora espa�os em branco
				if (ifs.peek() == ',')
					ifs.ignore(); // ignora v�rgula
				else if (ifs.peek() == ']')
					break; // fim da lista de v�rtices
			}
		}
		catch (...) {
			ifs.setstate(std::ios::failbit);
		}
		return ifs;
	}

}
