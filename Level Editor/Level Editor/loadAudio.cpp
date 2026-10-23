#include "loadAudio.h"

namespace {
	SoLoud::Wav* loadAudioFile(const char* path) {
		SoLoud::Wav* audio = new SoLoud::Wav;
		
		audio->load(path);
		
		return audio;
	}
}

std::map<std::string, SoLoud::Wav*> loadAudio(std::map<std::string, const char*> paths) {
	std::map<std::string, std::future<SoLoud::Wav*>> audioFutures;
	
	std::map<std::string, SoLoud::Wav*> audio;
	
	for(auto &entry : paths) {
		audioFutures.emplace(entry.first, std::async(std::launch::async, loadAudioFile, entry.second));
	}
	
	for(auto &entry : audioFutures) {
		audio.emplace(entry.first, entry.second.get());
	}
	
	return audio;
}
