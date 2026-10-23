#include "floor.h"

RepeatingFloor::RepeatingFloor(sf::Texture &texture) {
	texture.setRepeated(true);

	sprite.setTexture(texture);
}

void RepeatingFloor::update(const sf::RenderWindow &window) {
	sprite.setPosition(window.mapPixelToCoords(sf::Vector2i(0, 0)));
	sprite.setTextureRect(sf::IntRect(sf::Vector2i(window.getView().getCenter()), sf::Vector2i(window.getView().getSize())));
}

void RepeatingFloor::draw(sf::RenderWindow &window) {
	window.draw(sprite);
}
