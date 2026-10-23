#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "player.h"
#include "bullet.h"

class Player;

class Enemy {
	double x;
	double y;

	double r;

	double angle;

	int health = 30;

	static constexpr double speed = 1;

	sf::Sprite sprite;

public:
	Enemy(double x, double y);

	bool colliding(double x, double y, double r, double* dist = nullptr);

	void collideBullets(std::vector<Enemy> &enemies, int index, std::vector<Bullet> &bullets);

	void update(const Player &player, std::vector<Enemy> &enemies);
	void draw(sf::RenderWindow &window);
};
