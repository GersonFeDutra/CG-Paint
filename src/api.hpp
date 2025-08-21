#pragma once

// Ponte de importação para a API Gráfica OpenGL e a biblioteca GLUT

#if defined(_WIN32) || defined(_WIN64)
    // Windows
    #include <windows.h>
    #define TESS_CALLBACK CALLBACK
#elif defined(__APPLE__) || defined(__MACH__)
    static_assert(false, "Não implementado para macOS");
#elif defined(__linux__) || defined(__unix__) || defined(unix) || defined(__unix)
    #define TESS_CALLBACK
    // Linux
	#include <GL/glut.h>
#else
    static_assert(false, "Sistema operacional desconhecido");
#endif

#include <GL/freeglut.h> // Free GLUT moderno