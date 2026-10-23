#include "button.h"

#include <utility>

#include "assets.h"
#include "utils.h"
#include "collision.h"

Button::Button(int x, int y, const sf::String &text, std::function<void()> onClick) {
	sprite.setTexture(assets::sprites["pizza"]);

	sf::FloatRect bounds = utils::setOriginCenter(sprite);

	r = bounds.width / 2;

	sprite.setPosition(x, y);

	this->text.setFont(assets::fonts["Oswald-Regular"]);
	this->text.setString(text);
	this->text.setFillColor(sf::Color::Black);
	this->text.setOutlineThickness(2);
	this->text.setOutlineColor(sf::Color::White);

	utils::setOriginCenter(this->text);

	this->text.setPosition(x, y);

	this->onClick = std::move(onClick);
}

void Button::update(const sf::RenderWindow &window, bool mouseClicked) {
	sf::Vector2f position = window.mapPixelToCoords(sf::Mouse::getPosition(window));

	if(collision::dist(position.x, position.y, sprite.getPosition().x, sprite.getPosition().y) < r) {
		angle = std::lerp(angle, rotMax, animRate);
		scale = std::lerp(scale, scaleMax, animRate);

		if(mouseClicked) {
			onClick();
		}
	}else {
		angle = std::lerp(angle, 0, animRate);
		scale = std::lerp(scale, 1, animRate);
	}

	sprite.setRotation(angle);
	sprite.setScale(scale, scale);
	text.setRotation(angle);
	text.setScale(scale, scale);
}

void Button::draw(sf::RenderWindow &window) {
	window.draw(sprite);
	window.draw(text);
}
