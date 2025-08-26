#include <util.hpp>

#include "polygon.hpp"


namespace cg {

    // ponteiro acessível aos callbacks para salvar vértices criados pelo combine
    static thread_local std::vector<std::unique_ptr<GLdouble[]>>* currentStorage = nullptr;

    static void TESS_CALLBACK tessError(GLenum errorCode) {
        const GLubyte* err = gluErrorString(errorCode);
        print_error("Tessellation Error: %s\n", err);
    }


    static void TESS_CALLBACK tessCombine(const GLdouble coords[3], void* /*vertex_data*/[4],
        const GLfloat /*weight*/[4], void** outData) {
        if (!currentStorage) {
            // fallback: aloca e devolve (ainda seria um leak se s_currentStorage==nullptr)
            GLdouble* p = new GLdouble[3];
            p[0] = coords[0]; p[1] = coords[1]; p[2] = coords[2];
            *outData = p;
            return;
        }
        // cria um unique_ptr para o novo vértice e guarda no vetor
        auto up = std::unique_ptr<GLdouble[]>(new GLdouble[3]);
        up[0] = coords[0]; up[1] = coords[1]; up[2] = coords[2];
        void* raw = up.get();
        currentStorage->push_back(std::move(up));
        *outData = raw;
    }

    // Seleção de polígono (ray casting)
    // Determina se a posição do mouse está dentro de um polígono usando o algoritmo de "ray casting".
    bool Polygon::_isSelected(Vector2 mousePos) const
    {
        bool inside = false;
        size_t n = vertices.size();

        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            const Vector2& vi = vertices[i];
            const Vector2& vj = vertices[j];

            if (vi.y == vj.y && vi.y == mousePos.y) {
                // Ignora arestas horizontais na altura do clique
                continue;
            }

            // Verifica se o raio horizontal cruza a aresta
            if (((vi.y > mousePos.y) != (vj.y > mousePos.y))) {
                // Calcula a posição da interseção da aresta com o raio horizontal
                float intersectX = vj.x + (mousePos.y - vj.y) * (vi.x - vj.x) / (vi.y - vj.y);

                if (intersectX == mousePos.x) {
                    // Interseção exatamente em um vértice
                    bool otherUp = (vj.y > mousePos.y) != (vi.y > mousePos.y);
                    if (otherUp) {
                        inside = !inside; // Conta interseção apenas quando o outro vértice está para cima
                    }
                }
                else if (intersectX > mousePos.x) {
                    // Interseção à direita do ponto do clique
                    inside = !inside;
                }
            }
        }
        return inside;
    }


    void Polygon::_render() {

        GLdebug{
            glColor3f(innerColor.r, innerColor.g, innerColor.b);
        }
		ArrayList<Vector2> vertices;
		vertices.reserve(this->vertices.size());

		// Transforma os vértices do Sistema de Coordenadas Local para global usando a matriz de modelo
        for (const auto& vertice : this->vertices)
			vertices.push_back(model * vertice);

        switch (vertices.size()) {
            case 0: {
                assert_err("Cannot draw a polygon with no vertices");
            }; break;
            case 1: { // point
                GLdebug{
                    glPointSize(width);
                }
                GLdebug{
                    glBegin(GL_POINTS);
                        glVertex2f(vertices[0].x, vertices[0].y);
                    glEnd();
                }
            } break;
            case 2: { // line
                GLdebug{
				    glLineWidth(width);
                }
                GLdebug {
                    glBegin(GL_LINES);
                        glVertex2f(vertices[0].x, vertices[0].y);
                        glVertex2f(vertices[1].x, vertices[1].y);
                    glEnd();
                }
            } break;
            default: {
				// TODO -> Implementar contorno
				// TODO -> Usar cache para evitar tesselar a cada frame

                // container "dono" que guarda todas as alocações estáveis
                std::vector<std::unique_ptr<GLdouble[]>> tmp_vertices; // precisa ser alocado para converter no formato esperado

                // Tornamos o ponteiro acessível aos callbacks (thread_local para segurança mínima)
                currentStorage = &tmp_vertices;

                GLUtesselator* tess = gluNewTess();
                if (!tess) {
                    print_error("Failed to create GLU tesselator.\n");
                    currentStorage = nullptr;
                    return;
				}

                gluTessCallback(tess, GLU_TESS_BEGIN, (void (TESS_CALLBACK*)())glBegin);
                gluTessCallback(tess, GLU_TESS_END, (void (TESS_CALLBACK*)())glEnd);
                gluTessCallback(tess, GLU_TESS_VERTEX, (void (TESS_CALLBACK*)())glVertex3dv);
                gluTessCallback(tess, GLU_TESS_ERROR, (void (TESS_CALLBACK*)())tessError);
                gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid(TESS_CALLBACK*)()) &tessCombine);

                gluTessBeginPolygon(tess, nullptr);
                gluTessBeginContour(tess);

                // adiciona o ponto inicial e os vértices convertendo para alocações heap
                auto push_vertex = [&](GLdouble x, GLdouble y, GLdouble z = 0.0) {
                    auto up = std::unique_ptr<GLdouble[]>(new GLdouble[3]);
                    up[0] = x; up[1] = y; up[2] = z;
                    void* raw = up.get();
                    tmp_vertices.push_back(std::move(up));
                    gluTessVertex(tess, (GLdouble*)raw, raw);
                };

                for (const auto& vertice : vertices)
					push_vertex(vertice.x, vertice.y);

                // finaliza a tesselagem
                gluTessEndContour(tess);
                gluTessEndPolygon(tess);

                gluDeleteTess(tess);

				currentStorage = nullptr; // limpa o ponteiro thread_local
				// tmp_vertices é desalocado automaticamente ao sair do escopo
            }
        }
    }

    std::ostream& Polygon::_print(std::ostream& os) const
    {
        os << "Polygon: " << model << ", width: " << width << ", colors: [inner: " << innerColor << ", countour: " << countourColor << "], vertices[";

        auto vertice = vertices.begin();
        if (vertice < vertices.end()) {
            os << *vertice;
            for (vertice = ++vertice; vertice < vertices.end(); ++vertice)
                os << ", " << *vertice;
        }
        return os << ']';
    }

    std::ofstream& Polygon::_serialize(std::ofstream& ofs) const
    {
        ofs << model << " width: " << width << " colors: [inner: " << innerColor << " countour: " << countourColor << "] vertices[";

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

    std::ifstream& Polygon::_deserialize(std::ifstream& ifs)
    {
        try {
            std::string dummy;
			Transform2D newModel;
			float newWidth;
            Color colors[2];
			ArrayList<Vector2> newVertices;

			// Lê o cabeçalho
            if (!(ifs >> newModel >> dummy >> newWidth) || dummy != "width:" ||
                !(ifs >> dummy) || dummy != "colors:" ||
                !(ifs >> dummy) || dummy != "[inner:" ||
                !(ifs >> colors[0] >> dummy) || dummy != "countour:" ||
                !(ifs >> colors[1] >> dummy) || dummy != "]" ||
                !(ifs >> dummy) || dummy != "vertices[")
                ifs.setstate(std::ios::failbit); // marca falha no stream


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

			// Substitui os dados apenas se tudo foi lido corretamente
			model = newModel;
			width = newWidth;
			innerColor = colors[0];
			countourColor = colors[1];
            vertices = newVertices;
        }
        catch (...) {
            ifs.setstate(std::ios::failbit);
        }
        return ifs;
    }
}
