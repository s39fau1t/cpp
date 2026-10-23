#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

class Bullet {
	double angle;

	int removeTimer = 40;

	static constexpr double speed = 10;

public:
	double x;
	double y;

	int width;
	int height;

	double rotAngle = 0;

	int damage = 5;

	sf::Sprite sprite;

	Bullet(double x, double y, double angle);

	void update(std::vector<Bullet> &bullets, int index);
	void draw(sf::RenderWindow &window);
};
