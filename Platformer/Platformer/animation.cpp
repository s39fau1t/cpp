#include "animation.h"

//TODO: Maybe add padding support
Spritesheet::Spritesheet(const Image &image, int spriteWidth, int spriteHeight) : image(image) {
	this->spriteWidth = spriteWidth;
	this->spriteHeight = spriteHeight;
}

void Spritesheet::drawSprite(Window2D &window, int column, int row, float x, float y) {
	window.drawImage(image, column * spriteWidth, row * spriteHeight, spriteWidth, spriteHeight, x, y, spriteWidth, spriteHeight);
}

void Spritesheet::drawSprite(Window2D &window, int column, int row, float x, float y, float width, float height) {
	window.drawImage(image, column * spriteWidth, row * spriteHeight, spriteWidth, spriteHeight, x, y, width, height);
}
