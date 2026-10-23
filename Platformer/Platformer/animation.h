#ifndef animation_h
#define animation_h

#include <array>
#include <unordered_map>

#include "window2D.h"

struct Spritesheet {
	Image image;
	
	int spriteWidth;
	int spriteHeight;
	
	Spritesheet(const Image &image, int spriteWidth, int spriteHeight);
	
	void drawSprite(Window2D &window, int column, int row, float x, float y);
	void drawSprite(Window2D &window, int column, int row, float x, float y, float width, float height);
};

template <typename EnumType>
class AnimationController {
	Spritesheet spritesheet;
	
	std::unordered_map<EnumType, std::array<int, 4>> animations;
	
	double sPerFrame;
	
	double curTime;
	
public:
	int curX;
	int curY;
	
	EnumType state;
	
	AnimationController(const Image &image, int spriteWidth, int spriteHeight, std::unordered_map<EnumType, std::array<int, 4>> animations, EnumType initialState, int fps) : spritesheet(image, spriteWidth, spriteHeight) {
		this->animations = animations;
		
		state = initialState;
		
		curX = animations[state][0];
		curY = animations[state][1];
		
		sPerFrame = 1.0 / fps;
		
		curTime = glfwGetTime();
	}

	void changeState(EnumType state) {
		if(this->state != state) {
			this->state = state;
			
			curX = animations[state][0];
			curY = animations[state][1];
		}
	}

	void drawFrame(Window2D &window, float x, float y) {
		spritesheet.drawSprite(window, curX, curY, x, y);
		
		double time = glfwGetTime();
		
		if(time - curTime >= sPerFrame) {
			curTime = time;
			
			curX ++;
			
			if(curX > animations[state][2]) {
				curX = 0;
				
				curY ++;
				
				if(curY > animations[state][3]) {
					curX = animations[state][0];
					curY = animations[state][1];
				}
			}
		}
	}

	void drawFrame(Window2D &window, float x, float y, float width, float height) {
		spritesheet.drawSprite(window, curX, curY, x, y, width, height);
		
		double time = glfwGetTime();
		
		if(time - curTime >= sPerFrame) {
			curTime = time;
			
			curX ++;
			
			if(curX > animations[state][2]) {
				curX = 0;
				
				curY ++;
				
				if(curY > animations[state][3]) {
					curX = animations[state][0];
					curY = animations[state][1];
				}
			}
		}
	}
};

#endif
