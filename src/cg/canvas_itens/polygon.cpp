#include "polygon.hpp"

namespace cg
{
    void Polygon::_render() {
        if (vertices.empty()) {
            return;
        }
        GLdebug{
			glBegin(GL_POLYGON);
				glColor3ub(insideColor.r, insideColor.g, insideColor.b);
				glVertex2f(position.x, position.y);
				for (auto& vertice : vertices)
					glVertex2f(vertice.x, vertice.y);
			glEnd();
		}
    }
} // namespace cg
