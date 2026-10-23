#include <iostream>

#include <SFML/Graphics.hpp>

#include "assets.h"
#include "sound_player.h"
#include "player.h"
#include "enemy.h"
#include "enemy_manager.h"
#include "floor.h"
#include "rng.h"
#include "button.h"
#include "utils.h"
#include "letterbox.h"

enum class Scene {
	menu,
	game
} scene = Scene::menu;

constexpr int width = 800;
constexpr int height = 600;

int main() {
	//Load assets
	assets::load();

	//Seed RNG
	rng::seed();

	//Window
	sf::RenderWindow window(sf::VideoMode(width, height), "Gaming");
	window.setFramerateLimit(60);

	Player player(window.getSize().x / 2.0, window.getSize().y / 2.0);

	EnemyManager manager;

	RepeatingFloor floor(assets::sprites["floor_test"]);

	assets::music["background"]->setLoop(true);
	assets::music["background"]->play();

	sf::View worldView = window.getDefaultView();
	sf::View uiView = window.getDefaultView();

	/*
	sf::Shader drunk;
	drunk.loadFromFile("assets/shaders/drunk.frag", sf::Shader::Fragment);
	sf::Shader drunk2;
	drunk2.loadFromFile("assets/shaders/drunk2.frag", sf::Shader::Fragment);
	sf::Shader drunk3;
	drunk3.loadFromFile("assets/shaders/drunk3.frag", sf::Shader::Fragment);


	sf::Clock clock;
	*/

	bool mouseClicked = false;

	sf::Text title;
	title.setFont(assets::fonts["Oswald-Regular"]);
	title.setString("Title");
	title.setFillColor(sf::Color::White);
	utils::setOriginCenter(title);
	title.setPosition(400, 50);

	Button playButton(window.getSize().x / 2.0, window.getSize().y / 2.0, "Play", [&]() {
		scene = Scene::game;
	});

	while(window.isOpen()) {
		//Handle events
		sf::Event event;

		while(window.pollEvent(event)) {
			switch(event.type) {
				case sf::Event::Closed:
					window.close();

					break;
				case sf::Event::Resized:
					uiView.setSize(event.size.width, event.size.height);
					worldView.setSize(event.size.width, event.size.height);

					//uiView = getLetterboxView(uiView, event.size.width, event.size.height);
					//worldView = getLetterboxView(worldView, event.size.width, event.size.height);

					break;
				case sf::Event::MouseButtonReleased:
					if(event.mouseButton.button == sf::Mouse::Left) {
						mouseClicked = true;
					}

					break;
				default:
					break;
			}
		}

		window.clear(sf::Color::Black);

		switch(scene) {
			case Scene::menu:
				window.setView(uiView);

				window.draw(title);

				playButton.update(window, mouseClicked);
				playButton.draw(window);

				break;
			case Scene::game:
				player.update(window, worldView, manager);

				window.setView(worldView);

				floor.update(window);
				floor.draw(window);

				player.draw(window);

				manager.update(window, player);
				manager.draw(window);
		/*
				sf::Texture screenTexture;

				screenTexture.create(window.getSize().x, window.getSize().y);

				screenTexture.update(window);

				sf::Sprite screen(screenTexture);

				window.setView(window.getDefaultView());

				drunk.setUniform("resolution", sf::Vector2f(window.getSize()));
				drunk.setUniform("time", clock.getElapsedTime().asSeconds());
				drunk2.setUniform("resolution", sf::Vector2f(window.getSize()));
				drunk2.setUniform("time", clock.getElapsedTime().asSeconds());
				drunk3.setUniform("resolution", sf::Vector2f(window.getSize()));
				drunk3.setUniform("time", clock.getElapsedTime().asSeconds());

				window.draw(screen, &drunk);

				screenTexture.update(window);

				window.draw(screen, &drunk2);

				screenTexture.update(window);

				window.draw(screen, &drunk3);
*/
				break;
		}

		mouseClicked = false;

		window.display();

		soundPlayer::free();
	}
}
