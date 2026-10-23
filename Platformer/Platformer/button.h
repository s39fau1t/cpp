#ifndef button_h
#define button_h

#include <functional>

#include "window2D.h"

class Button {
	float x;
	float y;
	
	float width;
	float height;
	
	double animTimer = 0;
	
	std::function<double(double)> animInc;
	std::function<double(double)> animDec;
	
	std::function<void(float, float, float, float, double)> _draw;
	
	std::function<void()> onClick;
	
public:
	Button(float x, float y, float width, float height, std::function<double(double)> animInc, std::function<double(double)> animDec, std::function<void(float, float, float, float, double)> draw, std::function<void()> onClick);
	
	void update(double mouseX, double mouseY, bool mouseClick);
	void draw();
};

#endif
