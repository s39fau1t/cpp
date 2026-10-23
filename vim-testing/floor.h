#pragma once

#include <SFML/Graphics.hpp>

class RepeatingFloor {
	sf::Sprite sprite;

public:
	RepeatingFloor(sf::Texture &texture);

	void update(const sf::RenderWindow &window);
	void draw(sf::RenderWindow &window);
};
