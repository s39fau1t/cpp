#pragma once

#include <SFML/Audio.hpp>

namespace soundPlayer {
	void play(const sf::SoundBuffer &buffer);

	void free();
}
