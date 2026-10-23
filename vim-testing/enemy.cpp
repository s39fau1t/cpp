#include "enemy.h"

#include <cmath>
#include <iostream>

#include "assets.h"
#include "utils.h"
#include "sfml_should_use_radians_not_degrees.h"
#include "sound_player.h"
#include "collision.h"

Enemy::Enemy(double x, double y) {
	this->x = x;
	this->y = y;

	sprite.setTexture(assets::sprites["pizza"]);

	sf::FloatRect bounds = utils::setOriginCenter(sprite);

	r = bounds.width / 2.0;
}

bool Enemy::colliding(double x, double y, double r, double* dist) {
	return collision::circleCircle(this->x, this->y, this->r, x, y, r, dist);
}

void Enemy::collideBullets(std::vector<Enemy> &enemies, int index, std::vector<Bullet> &bullets) {
	for(int i = bullets.size() - 1; i >= 0; i --) {
		Bullet &bullet = bullets[i];

		if(collision::rotatedRectCircle(bullet.x, bullet.y, bullet.width, bullet.height, bullet.rotAngle, x, y, r)) {
			bullets.erase(bullets.begin() + i);

			health -= bullets[i].damage;

			soundPlayer::play(assets::sounds["splat"]);

			if(health <= 0) {
				enemies.erase(enemies.begin() + index);

				break;
			}
		}
	}
}

void Enemy::update(const Player &player, std::vector<Enemy> &enemies) {
	angle = std::atan2(player.y - y, player.x - x);

	x += std::cos(angle) * speed;
	y += std::sin(angle) * speed;

	for(Enemy &enemy : enemies) {
		if(this == &enemy) {
			continue;
		}

		double dist;

		//TODO: When an enemy collides, it moves, possibly causing another collision which is not handled in this frame
		if(enemy.colliding(x, y, r, &dist)) {
			//https://ericleong.me/research/circle-circle/
			double midpointX = (x + enemy.x) / 2;
			double midpointY = (y + enemy.y) / 2;

			x = midpointX + r * (x - enemy.x) / dist;
			y = midpointY + r * (y - enemy.y) / dist;

			enemy.x = midpointX + enemy.r * (enemy.x - x) / dist;
			enemy.y = midpointY + enemy.r * (enemy.y - y) / dist;
		}
	}

	sprite.setPosition(x, y);
	sprite.setRotation(radToDeg(angle));
}

void Enemy::draw(sf::RenderWindow &window) {
	window.draw(sprite);
}
