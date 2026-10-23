#include "sfml_should_use_radians_not_degrees.h"

#include <cmath>

double radToDeg(double rad) {
	return rad * 180 / M_PI;
}

double degToRad(double deg) {
	return deg * M_PI / 180;
}
