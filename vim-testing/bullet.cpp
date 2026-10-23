#include "bullet.h"

#include <cmath>

#include "assets.h"
#include "utils.h"

Bullet::Bullet(double x, double y, double angle) {
	this->x = x;
	this->y = y;

	this->angle = angle;

	sprite.setTexture(assets::sprites["bullet"]);

	sf::FloatRect bounds = utils::setOriginCenter(sprite);

	width = bounds.width;
	height = bounds.height;
}

void Bullet::update(std::vector<Bullet> &bullets, int index) {
	removeTimer --;

	if(removeTimer == 0) {
		bullets.erase(bullets.begin() + index);
	}

	x += std::cos(angle) * speed;
	y += std::sin(angle) * speed;

	rotAngle += speed;

	sprite.setPosition(x, y);
	sprite.setRotation(rotAngle);
}

void Bullet::draw(sf::RenderWindow &window) {
	window.draw(sprite);
}
