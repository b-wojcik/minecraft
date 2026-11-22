#pragma once

#define PROGRAM_DEBUG

#include <iostream>
#include <utility>
#include <cstdlib>

// !!! should be called only via the PROGRAM_ERROR() macro !!!
template<typename... Args>
void error(Args&&... args) {
	std::cerr << "PROGRAM_ERROR: ";
	((std::cerr << std::forward<Args>(args)), ...);
	std::cerr << '\n';

	std::exit(1);
}

// !!! should be called only via the PROGRAM_WARNING() macro !!!
template<typename... Args>
void warning(Args&&... args) {
	std::cerr << "PROGRAM_WARNING: ";
	((std::cerr << std::forward<Args>(args)), ...);
	std::cerr << '\n';
}

#ifdef PROGRAM_DEBUG
	// prints a message, then exits the program
	#define PROGRAM_ERROR(...) \
		error(__VA_ARGS__)

	// prints a warning
	#define PROGRAM_WARNING(...) \
		warning(__VA_ARGS__)
	#else
	// exits the program
	#define PROGRAM_ERROR(...) \
		(void)(std::exit(1))

	// does nothing
	#define PROGRAM_WARNING(...) \
		(void)(0)
#endif