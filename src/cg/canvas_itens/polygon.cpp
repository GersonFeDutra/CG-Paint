#include <util.hpp>
#include "polygon.hpp"


namespace cg {

    static void CALLBACK tessError(GLenum errorCode) {
        const GLubyte* err = gluErrorString(errorCode);
        print_error("Tessellation Error: %s\n", err);
    }

    void Polygon::_render() {

        GLdebug{
            glColor3f(innerColor.r, innerColor.g, innerColor.b);
        }

        switch (vertices.size()) {
            case 0: { // point
                glBegin(GL_POINTS);
                    glVertex2f(position.x, position.y);
                glEnd();
            } break;
            case 1: { // line
                glBegin(GL_POINTS);
                    glVertex2f(position.x, position.y);
                    glVertex2f(vertices[0].x, vertices[0].y);
                glEnd();
            } break;
            default: {
                std::vector<Vec3<GLdouble>> tmp_vertices; // precisa ser alocado para converter no formato esperado
				tmp_vertices.reserve(vertices.size() + 1);

                GLUtesselator* tess = gluNewTess();

                gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)())glBegin);
                gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)())glEnd);
                gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())glVertex2dv);
                gluTessCallback(tess, GLU_TESS_ERROR, (void (CALLBACK*)())tessError);

                gluTessBeginPolygon(tess, nullptr);
                gluTessBeginContour(tess);

                { // Position como ponto inicial
                    tmp_vertices.emplace_back(Vec3<GLdouble>(position));
                    gluTessVertex(tess, &tmp_vertices.back().x, &tmp_vertices.back().x);
                }
                for (auto vertice : vertices) {
                    tmp_vertices.emplace_back(Vec3<GLdouble>(vertice));
                    gluTessVertex(tess, &tmp_vertices.back().x, &tmp_vertices.back().x);
                }

                gluTessEndContour(tess);
                gluTessEndPolygon(tess);

                gluDeleteTess(tess);

            }
        }

        
        // GLdebug{
        //     // triangula se necess�rio
        //     // FIXME -> do not use a copy vector
        //     triangulation.triangulateIfNeeded(position, vertices);

        //     // desenha v�rios tri�ngulos formando um pol�gono:
        //     const auto& tverts = triangulation.verts();
        //     const auto& tidx = triangulation.indices();
        //     if (!tidx.empty()) {
        //         glBegin(GL_TRIANGLES);
        //         for (size_t i = 0; i < tidx.size(); ++i) {
        //             const Vector2& p = tverts[tidx[i]];
        //             glVertex2f(p.x, p.y);
        //         }
        //         glEnd();
        //     }
        //     // desenha contorno...
        // }
    }
}
