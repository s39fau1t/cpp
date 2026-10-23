#ifndef loadImages_h
#define loadImages_h

#include <future>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <unordered_map>

#include <stb/stb_image.h>

#include "window2D.h"

std::unordered_map<std::string, Image> loadImages(std::unordered_map<std::string, const char*> paths);

#endif
