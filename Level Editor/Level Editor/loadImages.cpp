#include "loadImages.h"

namespace {
	struct Texture {
		unsigned char* bytes;
		
		int width;
		int height;
		
		Texture(unsigned char* bytes, int width, int height) {
			this->bytes = bytes;
			
			this->width = width;
			this->height = height;
		}
		
		Texture(const Texture &that) {
			std::memcpy(bytes, that.bytes, strlen((char*)that.bytes) * sizeof(unsigned char));
			
			width = that.width;
			height = that.height;
		}
		
		Texture(Texture &&that) {
			bytes = that.bytes;
			that.bytes = nullptr;
			
			width = that.width;
			height = that.height;
		}
		
		Texture &operator=(Texture that) {
			std::swap(bytes, that.bytes);
			
			width = that.width;
			height = that.height;
			
			return *this;
		}
		
		~Texture() {
			stbi_image_free(bytes);
		}
	};

	Texture loadTexture(const char* path) {
		stbi_set_flip_vertically_on_load(true);
		
		int width, height, numColCh;
		
		unsigned char* bytes = stbi_load(path, &width, &height, &numColCh, 4);
		
		return Texture(bytes, width, height);
	}
}

std::map<std::string, Image> loadImages(std::map<std::string, const char*> paths) {
	std::map<std::string, std::future<Texture>> imageFutures;
	
	std::map<std::string, Image> images;
	
	for(auto &entry : paths) {
		imageFutures.emplace(entry.first, std::async(std::launch::async, loadTexture, entry.second));
	}
	
	for(auto &entry : imageFutures) {
		Texture texture = entry.second.get();
		
		images.emplace(entry.first, Image(texture.bytes, texture.width, texture.height));
	}
	
	return images;
}
