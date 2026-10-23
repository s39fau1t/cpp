#pragma once

#include <limits>

namespace rng {
	void seed();
	void seed(unsigned int seed);

	int randi(int min, int max = std::numeric_limits<int>::max());
	double randd(double min, double max = std::numeric_limits<double>::max());
};
