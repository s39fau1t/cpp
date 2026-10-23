#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace assets {
	extern std::unordered_map<std::string, sf::Texture> sprites;
	extern std::unordered_map<std::string, sf::Font> fonts;
	extern std::unordered_map<std::string, sf::SoundBuffer> sounds;
	extern std::unordered_map<std::string, std::unique_ptr<sf::Music>> music;

	void load();
};
