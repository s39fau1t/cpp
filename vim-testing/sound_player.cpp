#include "sound_player.h"

#include <deque>

namespace soundPlayer {
	namespace {
		//https://en.sfml-dev.org/forums/index.php?topic=10319.0
		std::deque<sf::Sound> sounds;
	}

	void play(const sf::SoundBuffer &buffer) {
		sounds.push_back(sf::Sound(buffer));

		sounds.back().play();
	}

	void free() {
		for(int i = sounds.size() - 1; i >= 0; i --) {
			if(sounds[i].getStatus() == sf::Sound::Stopped) {
				sounds.erase(sounds.begin() + i);
			}
		}
	}
}
