#include "enemy_manager.h"

#include "rng.h"

void EnemyManager::collideBullets(std::vector<Bullet> &bullets) {
	for(int i = enemies.size() - 1; i >= 0; i --) {
		enemies[i].collideBullets(enemies, i, bullets);
	}
}

void EnemyManager::update(const sf::RenderWindow &window, const Player &player) {
	enemyTimer --;

	if(enemyTimer == 0) {
		//TODO: Maybe don't make it dependant on screen size, or don't resize the screen

		double x;
		double y;

		int direction = rng::randi(0, 3);

		sf::Vector2f topLeft = window.mapPixelToCoords(sf::Vector2i(0, 0));
		sf::Vector2f bottomRight = window.mapPixelToCoords(sf::Vector2i(window.getSize()));

		switch(direction) {
			//Left
			case 0:
				x = rng::randd(topLeft.x - 100, topLeft.x);
				y = rng::randd(topLeft.y, bottomRight.y);

				break;
			//Right
			case 1:
				x = rng::randd(bottomRight.x, bottomRight.x + 100);
				y = rng::randd(topLeft.y, bottomRight.y);

				break;
			//Up
			case 2:
				x = rng::randd(topLeft.x, bottomRight.x);
				y = rng::randd(topLeft.y - 100, topLeft.y);

				break;
			//Down
			case 3:
				x = rng::randd(topLeft.x, bottomRight.x);
				y = rng::randd(bottomRight.y, bottomRight.y + 100);

				break;
		}

		enemies.push_back(Enemy(x, y));

		enemyTimer = enemyInterval;
	}

	for(Enemy &enemy : enemies) {
		enemy.update(player, enemies);
	}
}

void EnemyManager::draw(sf::RenderWindow &window) {
	for(Enemy &enemy : enemies) {
		enemy.draw(window);
	}
}
