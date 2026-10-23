#include <cmath>
#include <vector>
#include <chrono>
#include <functional>
#include <iostream>

#include <stb/stb_image.h>

#include "random.h"
#include "openGl2D.h"

using namespace std;

using namespace randomlib;

enum Scene {menu, game, death};

Scene scene = menu;

bool keys[348];

void updateKeys(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key != GLFW_KEY_UNKNOWN) {
		if(action == GLFW_PRESS) {
			keys[key] = true;
		}else if(action == GLFW_RELEASE) {
			keys[key] = false;
		}
	}
}

double mouseX = 0, mouseY = 0;

void updateMousePos(GLFWwindow* window, double xPos, double yPos) {
	mouseX = xPos;
	mouseY = yPos;
}

bool mouseDown = false, mouseClick = false;

void updateMouseButton(GLFWwindow* window, int button, int action, int mods) {
	if(action == GLFW_PRESS) {
		mouseDown = true;
	}else if(action == GLFW_RELEASE) {
		mouseDown = false;
		mouseClick = true;
	}
}

OpenGl2D ctx("Game", true);

class Arena {
public:
	int x;
	int y;
	int width;
	int height;
	
	Arena(int width, int height) {
		x = ctx.width / 2 - width / 2;
		y = ctx.height / 2 - height / 2;
		
		this->width = width;
		this->height = height;
	}
	
	void draw() {
		x = ctx.width / 2 - width / 2;
		y = ctx.height / 2 - height / 2;
		
		ctx.setFillColor(255, 255, 255);
		
		ctx.translate(x, y);
		
		ctx.rect(0, 0, width, height);
	}
};

Arena arena(1000, 1000);

bool circleCircleColl(int x1, int y1, int rad1, int x2, int y2, int rad2) {
	if(x1 + rad1 > x2 - rad2 && x1 - rad1 < x2 + rad2 && y1 + rad1 > y2 - rad2 && y1 - rad1 < y2 + rad2) {
		return true;
	}
	
	return false;
}

void button(int x, int y, int w, int h, function<void(int x, int y, int w, int h, bool mouseOver)> draw, function<void()> onClick) {
	bool mouseOver = false;
	
	if(mouseX > x && mouseX < x + w && mouseY > y && mouseY < y + h) {
		mouseOver = true;
	}
	
	draw(x, y, w, h, true);
	
	if(mouseOver && mouseClick) {
		onClick();
	}
}

class Particle {
private:
	double x;
	double y;
	
	int rad;
	
	double ang;
	
	float color[3];
	
public:
	double life;
	
	Particle(int x, int y, int rad, double ang, float r, float g, float b) {
		this->x = x;
		this->y = y;
		
		this->rad = rad;
		
		this->ang = ang;
		
		color[0] = r;
		color[1] = g;
		color[2] = b;
		
		life = 1;
	}
	
	void draw() {
		ctx.setFillColor(color[0], color[1], color[2]);
		
		ctx.globalAlpha = life;
		
		ctx.circle(x, y, rad);
		
		ctx.globalAlpha = 1;
	}
	
	void update() {
		x += cos(ang) * 3;
		y += sin(ang) * 3;
		
		life -= 0.05;
	}
};

vector<Particle> particles;

class Bullet {
public:
	double x;
	double y;
	
	double ang;
	
	int rad;
	
	Bullet(double x, double y, double ang) {
		this->x = x;
		this->y = y;
		
		rad = 5;
		
		this->ang = ang;
	}
	
	void draw() {
		ctx.setFillColor(0, 0, 0);
		
		ctx.circle(x, y, rad);
	}
	
	void update() {
		x += cos(ang) * 6;
		y += sin(ang) * 6;
	}
};

vector<Bullet> playerBullets;

class Player {
private:
	double ang;
	
	int load;
	
public:
	int x;
	int y;
	
	int health;
	
	int rad;
	
	Player() {
		x = arena.width / 2;
		y = arena.height / 2;
		
		rad = 10;
		
		ang = 0;
		
		load = 0;
		
		health = 100;
	}
	
	void draw() {
		ctx.setFillColor(125, 125, 125);
		
		ctx.globalAlpha = 0.5;
		
		ctx.rect(x - 15, y + 15, 30, 5);
		
		ctx.setFillColor(255, 0, 0);
		
		ctx.globalAlpha = 1;
		
		ctx.rect(x - 15, y + 15, health / 100.0 * 30.0, 5);
		
		ctx.setFillColor(0, 0, 0);
		
		ctx.circle(x, y, rad + 0.5);
		
		ctx.setFillColor(255, 219, 172);
		
		ctx.circle(x, y, rad);
		
		ctx.setFillColor(100, 100, 100);
		
		ctx.circle(x + cos(ang) * 12, y + sin(ang) * 12, 7);
		
		ctx.setFillColor(0, 0, 0);
		
		ctx.circle(x + cos(ang - 0.6) * 9, y + sin(ang - 0.6) * 9, 5.5);
		
		ctx.circle(x + cos(ang + 0.6) * 9, y + sin(ang + 0.6) * 9, 5.5);
		
		ctx.setFillColor(255, 219, 172);
		
		ctx.circle(x + cos(ang - 0.6) * 9, y + sin(ang - 0.6) * 9, 5);
		
		ctx.circle(x + cos(ang + 0.6) * 9, y + sin(ang + 0.6) * 9, 5);
	}
	
	void update() {
		ang = atan2(mouseY - y - arena.y, mouseX - x - arena.x);
		
		if(keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
			x -= 2;
		}
		
		if(keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
			x += 2;
		}
		
		if(keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
			y -= 2;
		}
		
		if(keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
			y += 2;
		}
		
		x = x - rad < 0 ? rad : x + rad > arena.width ? arena.width - rad : x;
		y = y - rad < 0 ? rad : y + rad > arena.height ? arena.height - rad : y;
		
		if(mouseDown && load <= 0) {
			playerBullets.push_back(Bullet(x + cos(ang) * 8, y + sin(ang) * 8, ang));
			
			load = 30;
		}
		
		if(health <= 0) {
			scene = death;
		}
		
		load --;
	}
};

Player player;

class Enemy {
private:
	int rad;
	
	double ang;
	
public:
	double x;
	double y;
	
	int health;
	
	Enemy(double x, double y) {
		this->x = x;
		this->y = y;
		
		ang = 0;
		
		rad = 10;
		
		health = 20;
	}
	
	void draw() {
		ctx.setFillColor(0, 0, 0);
		
		ctx.circle(x, y, rad);
		
		ctx.circle(x + cos(ang - 0.6) * 9, y + sin(ang - 0.6) * 9, 5);
		
		ctx.circle(x + cos(ang + 0.6) * 9, y + sin(ang + 0.6) * 9, 5);
	}
	
	void update() {
		ang = atan2(player.y - y, player.x - x);
		
		x += cos(ang);
		y += sin(ang);
		
		if(circleCircleColl(x, y, rad, player.x, player.y, player.rad)) {
			player.health -= 0.5;
		}
		
		for(int i = int(playerBullets.size() - 1); i >= 0; i --) {
			Bullet &bullet = playerBullets[i];
			
			if(circleCircleColl(x, y, rad, bullet.x, bullet.y, bullet.rad)) {
				for(int i = 0; i < 3; i ++) {
					particles.push_back(Particle(bullet.x, bullet.y, 3, random(bullet.ang + M_PI_2 + 0.4, bullet.ang + M_PI + M_PI_2 - 0.4), 0, 0, 0));
				}
				
				health -= 5;
				
				playerBullets.erase(playerBullets.begin() + i);
			}
		}
	}
};

vector<Enemy> enemies;

int main() {
	ctx.setKeyCallback(updateKeys);
	ctx.setMouseMoveCallback(updateMousePos);
	ctx.setMouseButtonCallback(updateMouseButton);
	
	int spawnRate = 300;
	
	int spawnTimer = 120;
	
	int enemiesKilled = 0;
	
	double msPerFrame = 1000.0 / 60.0;
	
	double prev = ctx.getTime() * 1000;
	
	double time = 0;
	
	while(ctx.isRunning()) {
		switch(scene) {
			case menu: {
				ctx.background(0, 0, 0, 1);
				
				button(ctx.width / 2 - 100, ctx.height / 2 - 25, 200, 50, [](int x, int y, int w, int h, bool mouseOver) {
					ctx.setFillColor(255, 255, 255);
					
					ctx.rect(x, y, w, h);
				}, []() {
					scene = game;
				});
				
				break;
			}
			
			case game: {
				double cur = ctx.getTime() * 1000;
				
				double elapsed = cur - prev;
				
				prev = cur;
				
				time += elapsed;
				
				while(time >= msPerFrame) {
					ctx.background(0, 0, 0, 1);
					
					arena.draw();
					
					for(int i = int(playerBullets.size() - 1); i >= 0; i --) {
						Bullet &bullet = playerBullets[i];
						
						bullet.update();
						bullet.draw();
						
						if(bullet.x - bullet.rad < 0 || bullet.x + bullet.rad > arena.width || bullet.y - bullet.rad < 0 || bullet.y + bullet.rad > arena.height) {
							for(int i = 0; i < 3; i ++) {
								particles.push_back(Particle(bullet.x, bullet.y, 3, random(bullet.ang + M_PI_2 + 0.4, bullet.ang + M_PI + M_PI_2 - 0.4), 0, 0, 0));
							}
							
							playerBullets.erase(playerBullets.begin() + i);
						}
					}
					
					for(int i = int(enemies.size() - 1); i >= 0; i --) {
						Enemy &enemy = enemies[i];
						
						enemy.update();
						enemy.draw();
						
						if(enemy.health <= 0) {
							for(int i = 0; i < 5; i ++) {
								particles.push_back(Particle(enemy.x, enemy.y, 5, random(0.0, M_PI * 2), 0, 0, 0));
							}
							
							enemiesKilled ++;
							
							enemies.erase(enemies.begin() + i);
						}
					}
					
					player.update();
					player.draw();
					
					for(int i = int(particles.size() - 1); i >= 0; i --) {
						Particle &particle = particles[i];
						
						particle.update();
						particle.draw();
						
						if(particle.life <= 0) {
							particles.erase(particles.begin() + i);
						}
					}
					
					if(spawnTimer <= 0) {
						int positions[4][2] = {{-60, random(0, arena.height)}, {arena.width + 60, random(0, arena.height)}, {random(0, arena.width), -60}, {random(0, arena.width), arena.height + 60}};
						
						int index = random(0, 4);
						
						int position[2];
						
						position[0] = positions[index][0];
						position[1] = positions[index][1];
						
						enemies.push_back(Enemy(position[0], position[1]));
						
						spawnTimer = spawnRate;
					}
					
					spawnTimer --;
					
					ctx.resetTranslation();
					
					time -= msPerFrame;
				}
				
				break;
			}
			
			case death: {
				ctx.background(255, 0, 0, 1);
				
				break;
			}
		}
		
		mouseClick = false;
		
		ctx.render();
	}
	
	ctx.terminate();
	return 0;
}
