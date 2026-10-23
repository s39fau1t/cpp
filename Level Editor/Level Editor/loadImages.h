#ifndef loadImages_h
#define loadImages_h

#include <future>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

#include <stb/stb_image.h>

#include "window2D.h"

std::map<std::string, Image> loadImages(std::map<std::string, const char*> paths);

#endif
