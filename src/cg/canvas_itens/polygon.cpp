#include "polygon.hpp"


namespace cg {

    void Polygon::_render() {


        if (vertices.size() < 2) {
            /* TODO -> desenha ponto/ linha */
            return;
        }

        GLdebug{
            glColor3f(innerColor.r, innerColor.g, innerColor.b);
        }
        GLdebug{
            // triangula se necessário
            // FIXME -> do not use a copy vector
            triangulation.triangulateIfNeeded(position, vertices);

            // desenha vários triângulos formando um polígono:
            const auto& tverts = triangulation.verts();
            const auto& tidx = triangulation.indices();
            if (!tidx.empty()) {
                glBegin(GL_TRIANGLES);
                for (size_t i = 0; i < tidx.size(); ++i) {
                    const Vector2& p = tverts[tidx[i]];
                    glVertex2f(p.x, p.y);
                }
                glEnd();
            }
            // desenha contorno...
        }
    }
}
