#ifndef loadAudio_h
#define loadAudio_h

#include <unordered_map>
#include <string>
#include <future>

#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

std::unordered_map<std::string, SoLoud::Wav*> loadAudio(std::unordered_map<std::string, const char*> paths);

#endif
