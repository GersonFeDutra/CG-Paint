#pragma once
// Ponte de importação para a API Gráfica OpenGL e a biblioteca GLUT + operações do SO

#include <string>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
    // Windows
    #include <windows.h>
    #include <ShlObj.h> // Para obter o caminhos do sistema
    #define TESS_CALLBACK CALLBACK

    inline std::string get_localdata_dir_path() {
        // Obtém o caminho do diretório de dados do aplicativo
        const char* appdata = std::getenv("APPDATA");
        return appdata ? std::string(appdata) : std::string(".");
    }
    inline std::string get_documents_dir_path() {
        // Obtém o caminho do diretório de documentos do usuário
        char path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path)))
            return std::string(path);
        else
            return std::string(".");
	}
#elif defined(__APPLE__) || defined(__MACH__)
    static_assert(false, "Não implementado para macOS");

    inline std::string get_localdata_dir_path() {
        // Obtém o caminho do diretório de dados do aplicativo
        const char* home = std::getenv("HOME");
        return home ? std::string(home) + "/Library/Application Support" : ".";
	}

    inline std::string get_documents_dir_path() {
        // Obtém o caminho do diretório de documentos do usuário
        const char* home = std::getenv("HOME");
        if (!home) return ".";
        // Tenta usar padrão XDG
        const char* xdg = std::getenv("XDG_DOCUMENTS_DIR");
        if (xdg) return std::string(xdg);
        // Fallback
        return std::string(home) + "/Documents";
    }
#elif defined(__linux__) || defined(__unix__) || defined(unix) || defined(__unix)
    #define TESS_CALLBACK
    // Linux
	#include <GL/glut.h>

    inline std::string get_localdata_dir_path() {
        // Obtém o caminho do diretório de dados do aplicativo
        const char* home = std::getenv("HOME");
        return home ? std::string(home) + "/.local/share" : ".";
	}

    inline std::string get_documents_dir_path() {
        // Obtém o caminho do diretório de documentos do usuário
        const char* home = std::getenv("HOME");
        if (!home) return ".";

        // Tenta usar padrão XDG
        const char* xdg = std::getenv("XDG_DOCUMENTS_DIR");
        if (xdg) return std::string(xdg);

        // Fallback
        return std::string(home) + "/Documents";
	}
#else
    static_assert(false, "Sistema operacional desconhecido");
#endif

#include <GL/freeglut.h> // Free GLUT moderno
