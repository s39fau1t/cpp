#include <iostream>
#include <string>
#include <cmath>
#include <limits>
#include <iomanip>
#include <stdexcept>

#include <gmpxx.h>

int getInt() {
	std::string str;
	
	std::getline(std::cin, str);
	
	return std::stoi(str);
}

int main() {
	int precision;
	
	std::cout << "Precision in bits (at least): ";
	
	try {
		precision = getInt();
	}catch(std::invalid_argument error) {
		std::cerr << "Precision must be a number\n";
	}
	
	int dice;
	
	std::cout << "Number of dice: ";
	
	try {
		dice = getInt();
	}catch(std::invalid_argument error) {
		std::cerr << "Number of dice must be a number\n";
		
		return -1;
	}
	
	mpf_class probability(5, precision);
	
	probability /= 6;
	
	mpf_pow_ui(probability.get_mpf_t(), probability.get_mpf_t(), dice);
	
	probability = 1 - probability;
	
	probability *= 100;
	
	mp_exp_t exp;
	
	std::string str = probability.get_str(exp);
	
	str.insert(exp, ".");
	
	std::cout << "Probability is: " << str << "%\n";
	
	return 0;
}
