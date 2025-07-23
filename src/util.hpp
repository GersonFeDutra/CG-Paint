#pragma once
/* Utilites header */

#include <cstdio>
#include <cassert>

#ifndef NDEBUG

#ifndef _DEBUG
#define _DEBUG
#endif

#endif // NDEBUG

// Use the _NDEBUG flag to pre-processor debug manipulation
#ifdef _DEBUG
constexpr bool IS_DEBUG = true;

template<typename... Args>
constexpr void assert_err(bool condition, const char* message = nullptr, Args ...args) {
	// TODO -> Log err message here
	//std::fprintf("\033[31m"); // unix
	if (message)
		std::fprintf(stderr, message, args...);
	//std::fprintf("\033[m"); // unix
	assert(condition);
}

#else
constexpr bool IS_DEBUG = false;
#endif // _DEBUG
