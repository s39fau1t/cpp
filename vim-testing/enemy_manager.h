#pragma once

#include <vector>

#include "enemy.h"
#include "bullet.h"
#include "player.h"

class Enemy;
class Player;

class EnemyManager {
	int enemyInterval = 120;
	int enemyTimer = enemyInterval;

public:
	std::vector<Enemy> enemies;

	void collideBullets(std::vector<Bullet> &bullets);

	void update(const sf::RenderWindow &window, const Player &player);
	void draw(sf::RenderWindow &window);
};
