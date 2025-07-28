#include "util.hpp"


inline void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

inline bool GLLogCall(const std::source_location location = std::source_location::current())
{
	bool out = true;
	while (GLenum error = glGetError()) {
		SET_CLI_RED();

		print_location_tag("OpenGL Error", location);
		fprintf(stderr, "Error Code: %d\n", error);

		RESET_CLI();
		out = false;
	}
	return out;
}