#pragma once

#include <SFML/Graphics.hpp>

namespace utils {
	template <typename T>
	sf::FloatRect setOriginCenter(T &obj) {
		sf::FloatRect bounds = obj.getLocalBounds();

		obj.setOrigin(bounds.width / 2, bounds.height / 2);

		return bounds;
	}
};
