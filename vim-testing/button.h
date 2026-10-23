#pragma once

#include <functional>

#include <SFML/Graphics.hpp>

//TODO
class Button {
	sf::Sprite sprite;
	sf::Text text;

	double r;

	double angle = 0;
	double scale = 1;

	std::function<void()> onClick;

	static constexpr double rotMax = 360;
	static constexpr double scaleMax = 1.2;
	static constexpr double animRate = 0.08;

public:
	Button(int x, int y, const sf::String &text, std::function<void()> onClick);

	void update(const sf::RenderWindow &window, bool mouseClicked);
	void draw(sf::RenderWindow &window);
};
