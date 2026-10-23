#include "assets.h"

#include <filesystem>

namespace assets {
	namespace {
		template <typename AssetType>
		static void loadFiles(const std::string &dir, std::unordered_map<std::string, AssetType> &map) {
			for(const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(dir)) {
				AssetType obj;

				obj.loadFromFile(entry.path());

				map.emplace(entry.path().stem(), obj);
			}
		}
	}

	std::unordered_map<std::string, sf::Texture> sprites;
	std::unordered_map<std::string, sf::Font> fonts;
	std::unordered_map<std::string, sf::SoundBuffer> sounds;
	std::unordered_map<std::string, std::unique_ptr<sf::Music>> music;

	void load() {
		loadFiles<sf::Texture>("assets/sprites", sprites);
		loadFiles<sf::Font>("assets/fonts", fonts);
		loadFiles<sf::SoundBuffer>("assets/audio/sounds", sounds);

		//TODO: Somehow use loadFiles() for this
		for(const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator("assets/audio/music")) {
			std::unique_ptr<sf::Music> musicFile = std::make_unique<sf::Music>();

			musicFile->openFromFile(entry.path());

			music.emplace(entry.path().stem(), std::move(musicFile));
		}
	}
}
