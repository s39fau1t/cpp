#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "bullet.h"
#include "enemy.h"
#include "enemy_manager.h"

class EnemyManager;

class Player {
	double health = 100;

	double r;

	double pizzaAngle = 0;
	double angle;

	static constexpr int bulletInterval = 50;
	int bulletTimer = 0;

	std::vector<Bullet> bullets;

	sf::Sprite pizza;
	sf::Sprite pineapple;

	static constexpr double speed = 2;

public:
	double x;
	double y;

	Player(double x, double y);

	void updateView(sf::View &view);

	void update(sf::RenderWindow &window, sf::View &worldView, EnemyManager &manager);
	void draw(sf::RenderWindow &window);
};
