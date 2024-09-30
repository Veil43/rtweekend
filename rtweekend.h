// rtweekend.h : Include file for standard system include files,
// or project specific include files.

#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

// C++ std usings

using std::make_shared;
using std::shared_ptr;

// constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degreesToRadians(double degrees) {
	return degrees * pi / 180;
}

inline double randomDouble() {
	// Returns a random real in [0, 1) for large RAND_MAX.
	return std::rand() / (RAND_MAX + 1.0);
}

inline double randomDouble(double min, double max) {
	// Returns a random real in range [min, max).
	return min + (max - min) * randomDouble();
}

// Common headers

#include "color.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#endif // !RTWEEKEND_H
