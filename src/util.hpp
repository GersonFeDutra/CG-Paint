#pragma once
/* Utilites header */
// TODO -> Log to file.

#include <cstdio>
#include <cassert>
#include <iostream>
#include <vector>
#include <source_location>

#include "api.hpp" // Includes the Open GL API, so you can include just this utilities header.


#if defined(_WIN32) || defined(_WIN64)
	extern HANDLE _hConsole;
	extern WORD _saved_attributes;
	#define SET_CLI_RED() SetConsoleTextAttribute(_hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY)
	#define SET_CLI_GREEN() SetConsoleTextAttribute(_hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY)
	#define SET_CLI_YELLOW() SetConsoleTextAttribute(_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)
	#define SET_CLI_BLUE() SetConsoleTextAttribute(_hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY)
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

template <typename T>
using ArrayList = std::vector<T>; // alias the data structure by the name of what it actually is.


template <typename... Args>
constexpr void print_warning(const char* message, Args...args) {
	SET_CLI_YELLOW();
	if constexpr (sizeof...(Args) > 0) {
		std::fprintf(stderr, message, std::forward<Args>(args)...); // segura, tipada
	}
	else {
		std::fprintf(stderr, "%s", message); // Mensagem formatada
	}
	fputc('\n', stderr);
	RESET_CLI();
}

template <typename... Args>
constexpr void print_error(const char* message, Args...args) {
	SET_CLI_RED();
	if constexpr (sizeof...(Args) > 0) {
		std::fprintf(stderr, message, std::forward<Args>(args)...); // segura, tipada
	}
	else {
		std::fprintf(stderr, "%s", message); // Mensagem formatada
	}
	fputc('\n', stderr);
	RESET_CLI();
}

template <typename... Args>
constexpr void print_success(const char* message, Args...args) {
	SET_CLI_GREEN();
	if constexpr (sizeof...(Args) > 0) {
		std::fprintf(stderr, message, std::forward<Args>(args)...); // segura, tipada
	}
	else {
		std::fprintf(stderr, "%s", message); // Mensagem formatada
	}
	fputc('\n', stderr);
	RESET_CLI();
}


#ifdef _MSC_VER
	#define my_assert(E) if (!(E)) __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
	#include <signal.h>
	#define my_assert(E) if (!(E)) raise(SIGTRAP)
#else
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
	#define GLdebug() if (GLDebugScope _gl_debug_scope = GLDebugScope())
	// Usa o escopo tempor�rio para realizar as chamadas no construtor/destrutor com RAII

	// Prefira a vers�o escopada acima
	//#define GLCall(GL) GLClearError(); GL; GLLogCall()

	#define print_var(VAR, ...) std::cerr << #VAR ##__VA_ARGS__ ": " << (VAR) << '\n'
#else
	constexpr bool IS_DEBUG = false; // Is debugger availlable?

	// Macro de Debug para chamadas OpenGL
	#define GLdebug() do {} while(false)
	//#define GLCall(GL) GL

	#define print_var(VAR, ...) std::cerr << #VAR ##__VA_ARGS__ ": " << (VAR) << '\n'
#endif // _DEBUG


// Prints a warning message
template<typename... Args>
void warn(const char* message, Args... args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		SET_CLI_YELLOW();

		print_location_tag("Warning", location);
		if constexpr (sizeof...(Args) > 0) {
			std::fprintf(stderr, message, std::forward<Args>(args)...); // segura, tipada
		}
		else {
			std::fprintf(stderr, "%s", message); // Mensagem formatada
		}

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
		if constexpr (sizeof...(Args) > 0) {
			std::fprintf(stderr, message, std::forward<Args>(args)...); // segura, tipada
		}
		else {
			std::fprintf(stderr, "%s", message); // Mensagem formatada
		}

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

// Prints a success message
template<typename... Args>
void suc(const char* message, Args... args,
	const std::source_location location = std::source_location::current()
) {
	if constexpr (IS_DEBUG) {
		SET_CLI_YELLOW();

		print_location_tag("Success", location);
		if constexpr (sizeof...(Args) > 0) {
			std::fprintf(stderr, message, std::forward<Args>(args)...); // segura, tipada
		}
		else {
			std::fprintf(stderr, "%s", message); // Mensagem formatada
		}

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
inline constexpr void _assert_err(bool condition,
	const std::source_location location = std::source_location::current(),
	const char* message = nullptr, Args ...args
) {
	if constexpr (IS_DEBUG) {
		if (!condition) {
			SET_CLI_YELLOW();

			print_location_tag("Assertion", location);
			if (message) {
				if constexpr (sizeof...(Args) > 0) {
					std::fprintf(stderr, message, std::forward<Args>(args)...); // segura, tipada
				}
				else {
					std::fprintf(stderr, "%s", message); // Mensagem formatada
				}
			}
			my_assert(condition);

			RESET_CLI();
		}
	}
}

#define assert_err(cond, ...) do { \
        if (!(cond)) { \
            _assert_err((false), std::source_location::current(), ##__VA_ARGS__); \
            my_assert(false); /* <- para exatamente na linha da chamada */ \
        } \
    } while (0)
