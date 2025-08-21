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

    void Polygon::_render() {

        GLdebug{
            glColor3f(innerColor.r, innerColor.g, innerColor.b);
        }

        switch (vertices.size()) {
            case 0: { // point
                glPointSize(1.0f); // TODO -> Add line width

                glBegin(GL_POINTS);
                    glVertex2f(position.x, position.y);
                glEnd();
            } break;
            case 1: { // line
                glBegin(GL_LINES);
                    glVertex2f(position.x, position.y);
                    glVertex2f(vertices[0].x, vertices[0].y);
                glEnd();
            } break;
            default: {
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

                {
                    // Position como vértice inicial
				    push_vertex(position.x, position.y);
                }
                for (auto vertice : vertices)
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
}
