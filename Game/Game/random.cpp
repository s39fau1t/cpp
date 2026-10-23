#include "random.h"

random_device device;

mt19937 generator(device());

int randomlib::random(int min, int max) {
	uniform_int_distribution<int> dist(min, max);
	
	return dist(generator);
}

double randomlib::random(double min, double max) {
	uniform_real_distribution<double> dist(min, max);
	
	return dist(generator);
}
