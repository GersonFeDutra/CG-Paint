#include "polygon.hpp"


namespace cg {

    void Polygon::_render() {
        // TODO -> do not use a copy vector
        std::vector<Vector2> pts;
        pts.push_back(position);
        for (auto& v : vertices) pts.push_back(v);

        if (pts.size() < 3) {
            /* TODO -> desenha ponto/ linha */
            return;
        }

        GLdebug{
            glColor3f(innerColor.r, innerColor.g, innerColor.b);
        }
        GLdebug{
            // triangula se necessário
            triangulation.triangulateIfNeeded(pts);

            // desenha vários triângulos formando um polígono:
            const auto& tverts = triangulation.verts();
            const auto& tidx = triangulation.indices();
            if (!tidx.empty()) {
                glBegin(GL_TRIANGLES);
                for (size_t i = 0; i + 2 < tidx.size(); ++i) {
                    const Vector2& p = tverts[tidx[i]];
                    glVertex2f(p.x, p.y);
                }
                glEnd();
            }
            // desenha contorno...
        }
    }
}
