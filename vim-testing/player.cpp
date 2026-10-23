#include "player.h"

#include <cmath>

#include "assets.h"
#include "utils.h"
#include "sound_player.h"
#include "sfml_should_use_radians_not_degrees.h"





#include <iostream>

Player::Player(double x, double y) {
	this->x = x;
	this->y = y;

	pizza.setTexture(assets::sprites["pizza"]);
	pineapple.setTexture(assets::sprites["pineapple"]);

	sf::FloatRect pizzaBounds = utils::setOriginCenter(pizza);

	r = pizzaBounds.width / 2.0;

	utils::setOriginCenter(pineapple);
}

void Player::update(sf::RenderWindow &window, sf::View &view, EnemyManager &manager) {
	sf::Vector2<double> vel;

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		vel.x -= 2;
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		vel.x += 2;
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		vel.y -= 2;
	}

	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		vel.y += 2;
	}

	double magnitude = std::hypot(vel.x, vel.y);

	if(magnitude > 0) {
		vel /= magnitude;
		vel *= speed;
	}

	x += vel.x;
	y += vel.y;

	view.setCenter(x, y);
	window.setView(view);

	sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

	angle = std::atan2(mouse.y - y, mouse.x - x);

	if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && bulletTimer <= 0) {
		bullets.push_back(Bullet(x, y, angle));

		soundPlayer::play(assets::sounds["shoot"]);

		bulletTimer = bulletInterval;
	}

	pizzaAngle += 0.3;

	bulletTimer --;

	for(int i = bullets.size() - 1; i >= 0; i --) {
		bullets[i].update(bullets, i);
	}

	manager.collideBullets(bullets);

	for(Enemy &enemy : manager.enemies) {
		if(enemy.colliding(x, y, r)) {
			health -= 0.2;

			if(health <= 0) {
				//TODO: Death
				std::cout << "Death!\n";
			}

			//So that damage doesn't stack the more enemies you are touching
			break;
		}
	}

	pizza.setPosition(x, y);
	pizza.setRotation(pizzaAngle);

	pineapple.setPosition(x, y);
	pineapple.setRotation(radToDeg(angle) + 90);
}

void Player::draw(sf::RenderWindow &window) {
	window.draw(pizza);

	for(Bullet &bullet : bullets) {
		bullet.draw(window);
	}

	window.draw(pineapple);
}
