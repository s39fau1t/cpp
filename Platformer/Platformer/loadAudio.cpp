#include "loadAudio.h"

namespace {
	SoLoud::Wav* loadAudioFile(const char* path) {
		SoLoud::Wav* audio = new SoLoud::Wav;
		
		audio->load(path);
		
		return audio;
	}
}

std::unordered_map<std::string, SoLoud::Wav*> loadAudio(std::unordered_map<std::string, const char*> paths) {
	std::unordered_map<std::string, std::future<SoLoud::Wav*>> audioFutures;
	
	std::unordered_map<std::string, SoLoud::Wav*> audio;
	
	for(auto &entry : paths) {
		audioFutures.emplace(entry.first, std::async(std::launch::async, loadAudioFile, entry.second));
	}
	
	for(auto &entry : audioFutures) {
		audio.emplace(entry.first, entry.second.get());
	}
	
	return audio;
}
