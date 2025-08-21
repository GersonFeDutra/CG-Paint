#pragma once
/* Utilites header */
// TODO -> Log to file.

#include <cstdio>
#include <cassert>
#include <iostream>
#include <source_location>

#include "api.hpp" // Includes the Open GL API, so you can include just this utilities header.


#if defined(_WIN32) || defined(_WIN64)
	extern HANDLE _hConsole;
	extern WORD _saved_attributes;
	#define SET_CLI_RED() SetConsoleTextAttribute(_hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY)
	#define SET_CLI_GREEN() SetConsoleTextAttribute(_hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY)
	#define SET_CLI_YELLOW() SetConsoleTextAttribute(_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
	#define SET_CLI_BLUE() SetConsoleTextAttribute(_hConsole, FOREGROUND_BLUE FOREGROUND_INTENSITY)
	#define RESET_CLI() SetConsoleTextAttribute(_hConsole, _saved_attributes)
#elif defined(__APPLE__) || defined(__MACH__)
	static_assert(false, "Não implementado para macOS");
#elif defined(__linux__) || defined(__unix__) || defined(unix) || defined(__unix)
	#define SET_CLI_RED() std::cerr << "\033[31m"
	#define SET_CLI_GREEN() std::cerr << "\033[32m"
	#define SET_CLI_YELLOW() std::cerr << "\033[33m"
	#define SET_CLI_BLUE() std::cerr << "\033[34m"
	#define RESET_CLI() std::cerr << "\033[m"
#else
	static_assert(false, "Sistema operacional desconhecido");
#endif


template <typename... Args>
constexpr void print_warning(const char* message, Args...args) {
	SET_CLI_YELLOW();
	fprintf(stderr, message, args...);
	fputc('\n', stderr);
	RESET_CLI();
}

template <typename... Args>
constexpr void print_error(const char* message, Args...args) {
	SET_CLI_RED();
	fprintf(stderr, message, args...);
	fputc('\n', stderr);
	RESET_CLI();
}

template <typename... Args>
constexpr void print_success(const char* message, Args...args) {
	SET_CLI_GREEN();
	fprintf(stderr, message, args...);
	fputc('\n', stderr);
	RESET_CLI();
}


#ifdef _MSC_VER
	#define my_assert(E) if (!(E)) __debugbreak();
#else
	#include <cassert>
	#define my_assert(E) assert(E)
#endif


#ifndef NDEBUG
	// Use the _DEBUG flag to pre-processor debug manipulation
	#ifndef _DEBUG
		#define _DEBUG
	#endif
#endif // NDEBUG


// Prints a source file location in the stderr to mark the next message (this does not break the line with '\n').
inline void print_location_tag(const char* tag,
	const std::source_location location = std::source_location::current()
) {
	fprintf(stderr, "[%s] %s:%d (%s): ", // Arquivo, linha e fun��o
		tag, location.file_name(), location.line(), location.function_name());
}


/* Clear Open GL Error codes stack. */
inline void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}


/* Logs every Open GL error code accumulated. */
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


#ifdef _DEBUG
	constexpr bool IS_DEBUG = true;

	class GLDebugScope {
	public:
		GLDebugScope(const std::source_location loc = std::source_location::current())
			: location(loc) {
			GLClearError();
		}
		~GLDebugScope() {
			GLLogCall(location);
		}

		operator bool() { return true; }

	private:
		std::source_location location;
	};

	/* Macro de Debug para chamadas OpenGL */
	#define GLdebug if (GLDebugScope _gl_debug_scope = GLDebugScope())
	// Usa o escopo tempor�rio para realizar as chamadas no construtor/destrutor com RAII

	// Prefira a vers�o escopada acima
	//#define GLCall(GL) GLClearError(); GL; GLLogCall()
#else
	constexpr bool IS_DEBUG = false; // Is debugger availlable?

	// Macro de Debug para chamadas OpenGL
	#define GLdebug
	//#define GLCall(GL) GL
#endif // _DEBUG


// Prints a warning message
template<typename... Args>
void warn(const char* message, Args... args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		SET_CLI_YELLOW();

		print_location_tag("Warning", location);
		fprintf(stderr, message, args...); // Mensagem formatada

		RESET_CLI();
	}
}

// Prints a warning message within a given condition
template<typename... Args>
void warn(bool condition, const char* message, Args... args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		if (condition) {
			warn(message, args..., location);
		}
	}
}


// Prints an error message
template<typename... Args>
void err(const char* message, Args... args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		SET_CLI_YELLOW();

		print_location_tag("Error", location);
		fprintf(stderr, message, args...); // Mensagem formatada

		RESET_CLI();
	}
}

// Prints a err message within a given condition
template<typename... Args>
void err(bool condition, const char* message, Args... args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		if (condition) {
			err(message, args..., location);
		}
	}
}

// Prints a sucess message
template<typename... Args>
void suc(const char* message, Args... args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		SET_CLI_YELLOW();

		print_location_tag("Success", location);
		fprintf(stderr, message, args...); // Mensagem formatada

		RESET_CLI();
	}
}

// Prints a success message within a given condition
template<typename... Args>
void suc(bool condition, const char* message, Args... args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		if (condition) {
			suc(message, args..., location);
		}
	}
}


// Asserts the condition.
template<typename... Args>
constexpr void assert_err(bool condition, const char* message = nullptr, Args ...args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		if (!condition) {
			SET_CLI_YELLOW();

			print_location_tag("Assertion", location);
			if (message)
				fprintf(stderr, message, args...); // Mensagem formatada
			my_assert(condition);

			RESET_CLI();
		}
	}
}
