#include "rng.h"

#include <random>

namespace rng {
	namespace {
		std::mt19937 rand;
	};

	void seed() {
		rand.seed((std::random_device())());
	}

	void seed(unsigned int seed) {
		rand.seed(seed);
	}

	int randi(int min, int max) {
		std::uniform_int_distribution dist(min, max);

		return dist(rand);
	}

	double randd(double min, double max) {
		std::uniform_real_distribution dist(min, max);

		return dist(rand);
	}
};
