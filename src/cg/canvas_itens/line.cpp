#include "line.hpp"

namespace cg {

	void Line::_render()
	{
		if (vertices.empty())
			return; // A linha deve ter pelo menos 2 vértices (a posição do item conta como 1 vértice)
		GLdebug{
			glBegin(GL_LINE_STRIP);
				glColor3f(color.r, color.g, color.b);
				glVertex2f(position.x, position.y);
				for (auto& vertice : vertices)
					glVertex2f(vertice.x, vertice.y);
			glEnd();
		}
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
			ofs << ' '; // se não houver vértices, imprime separador vazio
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
