#include "button.h"

Button::Button(float x, float y, float width, float height, std::function<double(double)> animInc, std::function<double(double)> animDec, std::function<void(float, float, float, float, double)> draw, std::function<void()> onClick) {
	this->x = x;
	this->y = y;
	
	this->width = width;
	this->height = height;
	
	this->animInc = animInc;
	this->animDec = animDec;
	
	_draw = draw;
	
	this->onClick = onClick;
}

void Button::update(double mouseX, double mouseY, bool mouseClick) {
	bool over = mouseX > x && mouseX < x + width && mouseY > y && mouseY < y + height;
	
	if(over) {
		animTimer = animInc(animTimer);
		
		if(mouseClick) {
			animTimer = 0;
			
			onClick();
		}
	}else {
		if(animTimer > 0) {
			animTimer = animDec(animTimer);
		}
	}
}

void Button::draw() {
	_draw(x, y, width, height, animTimer);
}
