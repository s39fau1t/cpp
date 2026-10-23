#ifndef loadAudio_h
#define loadAudio_h

#include <map>
#include <string>
#include <future>

#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

std::map<std::string, SoLoud::Wav*> loadAudio(std::map<std::string, const char*> paths);

#endif
