#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>

#include <json/json.h>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

#include "window2D.h"
#include "button.h"
#include "animation.h"
#include "loadImages.h"
#include "loadAudio.h"

#include "emscripten.h"

using json = nlohmann::json;

//https://www.fontspace.com/pixeloid-font-f69232

//TODO: Fix image flickering on edges of images

Window2D window(800, 800, "Platformer", Window2D::StretchMode::stretchKeepAspect/*, true*/);

enum class Scene {menu, game, win};

Scene scene = Scene::menu;

double mouseX = 0;
double mouseY = 0;

bool mouseDown = false;
bool mousePress = false;
bool mouseClick = false;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if(action == GLFW_PRESS) {
		mouseDown = true;
		mousePress = true;
	}else {
		mouseDown = false;
		mouseClick = true;
	}
}

bool keys[348];

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(action == GLFW_PRESS) {
		keys[key] = true;
	}else if(action == GLFW_RELEASE) {
		keys[key] = false;
	}
}

std::unordered_map<std::string, Image> images = loadImages({{"player", "assets/images/player.png"}, {"sandBlock", "assets/images/sand_block.png"}, {"lockedBlock", "assets/images/locked_block.png"}, {"key", "assets/images/key.png"}, {"sandBlockWall", "assets/images/sand_block_wall.png"}, {"spike", "assets/images/spike.png"}, {"downSpike", "assets/images/down_spike.png"}, {"gem", "assets/images/gem.png"}});

//soloud library object
SoLoud::Soloud soloud;

//I have to allocate these on the heap because they "are not meant to be copied." https://github.com/jarikomppa/soloud/issues/283 SERIOUSLY HOW HARD IS IT TO ADD A COPY CONSTRUCTOR
std::unordered_map<std::string, SoLoud::Wav*> audio = loadAudio({{"step", "assets/audio/step.wav"}, {"jump", "assets/audio/jump.wav"}, {"pickupKey", "assets/audio/pickup_key.wav"}, {"unlock", "assets/audio/unlock.wav"}, {"death", "assets/audio/death.wav"}, {"button", "assets/audio/button.wav"}});

//https://gamedev.stackexchange.com/questions/26004/how-to-detect-2d-line-on-line-collision
bool lineLineCollide(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
	double uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	double uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
	
	if(uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		return true;
	}

	return false;
}

//Does not check if triangle is inside, but I don't need that
bool triangleRectCollide(double tx1, double ty1, double tx2, double ty2, double tx3, double ty3, double rx, double ry, double rwidth, double rheight) {
	return lineLineCollide(tx1, ty1, tx2, ty2, rx, ry, rx + rwidth, ry) || lineLineCollide(tx1, ty1, tx2, ty2, rx, ry, rx, ry + rheight) || lineLineCollide(tx1, ty1, tx2, ty2, rx + rwidth, ry, rx + rwidth, ry + rheight) || lineLineCollide(tx1, ty1, tx2, ty2, rx, ry + rheight, rx + rwidth, ry + rheight) || lineLineCollide(tx2, ty2, tx3, ty3, rx, ry, rx + rwidth, ry) || lineLineCollide(tx2, ty2, tx3, ty3, rx, ry, rx, ry + rheight) || lineLineCollide(tx2, ty2, tx3, ty3, rx + rwidth, ry, rx + rwidth, ry + rheight) || lineLineCollide(tx2, ty2, tx3, ty3, rx, ry + rheight, rx + rwidth, ry + rheight) || lineLineCollide(tx3, ty3, tx1, ty1, rx, ry, rx, ry + rheight) || lineLineCollide(tx3, ty3, tx1, ty1, rx + rwidth, ry, rx + rwidth, ry + rheight) || lineLineCollide(tx3, ty3, tx1, ty1, rx, ry + rheight, rx + rwidth, ry + rheight);
}

bool collide(double x1, double y1, int width1, int height1, double x2, double y2, int width2, int height2) {
	return x1 + width1 > x2 && x1 < x2 + width2 && y1 + height1 > y2 && y1 < y2 + height2;
}

bool adjacent(double x1, double y1, int width1, int height1, double x2, double y2, int width2, int height2) {
	return x1 + width1 + 1 > x2 && x1 < x2 + width2 + 1 && y1 + height1 + 1 > y2 && y1 < y2 + height2 + 1;
}

double dist(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

const std::vector<std::string> blockTypeStrings = {"sandBlock", "lockedBlock", "key", "sandBlockWall", "spike", "downSpike", "gem", "barrier"};

enum class BlockType {sandBlock, lockedBlock, key, sandBlockWall, spike, downSpike, gem, barrier};

class Player;

class Block {
public:
	int x;
	int y;
	
	int width;
	int height;
	
	bool collision;
	
	BlockType type;
	
	Block(int x, int y, BlockType type, bool collision = true, int width = 16, int height = 16) {
		this->x = x;
		this->y = y;
		
		this->type = type;
		
		this->collision = collision;
		
		this->width = width;
		this->height = height;
	}
	
	virtual void draw() {};
	virtual void onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) {};
};

class SandBlock : public Block {
public:
	SandBlock(int x, int y) : Block(x, y, BlockType::sandBlock) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("sandBlock"), x, y, width, height);
	}
};

class LockedBlock : public Block {
public:
	LockedBlock(int x, int y) : Block(x, y, BlockType::lockedBlock) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("lockedBlock"), x, y, width, height);
	}
	
	void onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class Key : public Block {
public:
	Key(int x, int y) : Block(x, y, BlockType::key, false, 16, 7) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("key"), x, y, width, height);
	}
	
	void onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class SandBlockWall : public Block {
public:
	SandBlockWall(int x, int y) : Block(x, y, BlockType::sandBlockWall, false) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("sandBlockWall"), x, y, width, height);
	}
};

class Spike : public Block {
public:
	Spike(int x, int y) : Block(x, y, BlockType::spike, false) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("spike"), x, y, width, height);
	}
	
	void onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class DownSpike : public Block {
public:
	DownSpike(int x, int y) : Block(x, y, BlockType::downSpike, false) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("downSpike"), x, y, width, height);
	}
	
	void onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class Gem : public Block {
	int t = 0;
	
public:
	Gem(int x, int y) : Block(x, y, BlockType::gem, false) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("gem"), x, y - 2 + std::sin(t / 10.0) * 2, width, height);
		
		t ++;
	}
	
	void onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class Barrier : public Block {
public:
	Barrier(int x, int y) : Block(x, y, BlockType::barrier) {
		
	}
};

std::string blockTypeToString(BlockType type) {
	return blockTypeStrings[static_cast<int>(type)];
}

BlockType stringToBlockType(const std::string &str) {
	std::vector<std::string>::const_iterator it = std::find(blockTypeStrings.begin(), blockTypeStrings.end(), str);
	
	if(it != blockTypeStrings.end()) {
		return static_cast<BlockType>(std::distance(blockTypeStrings.begin(), it));
	}
	
	throw std::invalid_argument("String is not a valid block type");
}

Block* createBlock(BlockType type, int x, int y) {
	Block* block;
	
	switch (type) {
		case BlockType::sandBlock:
			block = new SandBlock(x, y);
			break;
		case BlockType::lockedBlock:
			block = new LockedBlock(x, y);
			break;
		case BlockType::key:
			block = new Key(x, y);
			break;
		case BlockType::sandBlockWall:
			block = new SandBlockWall(x, y);
			break;
		case BlockType::spike:
			block = new Spike(x, y);
			break;
		case BlockType::downSpike:
			block = new DownSpike(x, y);
			break;
		case BlockType::gem:
			block = new Gem(x, y);
			break;
		case BlockType::barrier:
			block = new Barrier(x, y);
			break;
	}
	
	return block;
}

void loadBlocks(json &worldMap, std::vector<Block*> &blocks, std::string_view x, std::string_view y, float bg[3]) {
	for(Block* block : blocks) {
		delete block;
	}
	
	blocks.clear();
	
	//TODO: Maybe get rid of this
	if(worldMap.find(y) == worldMap.end()) {
		worldMap[y] = json::object();
	}
	
	//TODO: And this
	if(worldMap[y].find(x) == worldMap[y].end()) {
		worldMap[y][x] = {
			{"bg", {bg[0], bg[1], bg[2]}},
			{"blocks", {}}
		};
	}
	
	for(json::object_t block : worldMap[y][x]["blocks"]) {
		blocks.push_back(createBlock(stringToBlockType(block["type"]), block["x"], block["y"]));
	}
}

void addToMap(json &worldMap, const std::vector<Block*> &blocks, std::string_view x, std::string_view y) {
	worldMap[y][x]["blocks"].clear();
	
	for(Block* block : blocks) {
		worldMap[y][x]["blocks"].push_back({
			{"x", block->x},
			{"y", block->y},
			{"type", blockTypeToString(block->type)},
		});
	}
}

void moveArea(json &worldMap, std::vector<Block*> &blocks, int &x, int &y, int movX, int movY) {
	std::string strX = std::to_string(x);
	std::string strY = std::to_string(y);
	
	float bg[3] = {worldMap[strY][strX]["bg"][0], worldMap[strY][strX]["bg"][1], worldMap[strY][strX]["bg"][2]};
	
	addToMap(worldMap, blocks, strX, strY);
	
	x += movX;
	y += movY;
	
	strX = std::to_string(x);
	strY = std::to_string(y);
	
	loadBlocks(worldMap, blocks, strX, strY, bg);
}

class WinInfo {
	int t = 0;
	
	double start;
	double time;
	
	std::vector<int> times = {150, 300, 450, 700};
	
	std::vector<bool> state = {false, false, false, false};
	
public:
	WinInfo() {
		
	}
	
	void startTimer() {
		start = glfwGetTime();
	}
	
	void endTimer() {
		time = glfwGetTime() - start;
	}
	
	void update() {
		t ++;
		
		std::vector<bool>::iterator nextStateIt = std::find(state.begin(), state.end(), false);
		
		if(nextStateIt != state.end()) {
			int nextState = static_cast<int>(static_cast<int>(std::distance(state.begin(), nextStateIt)));
			
			if(t >= times[nextState]) {
				soloud.play(*audio["unlock"]);
				
				state[nextState] = true;
			}
		}
	}
	
	void draw(int deaths, Button &backButton) {
		window.fill(0, 0, 0);
		
		window.textAlign = Window2D::TextAlign::center;
		
		if(state[0]) {
			window.textSize = 50;
			
			window.text("You Win!", window.width / 2, 80);
		}
		if(state[1]) {
			window.textSize = 30;
			
			window.text("You died " + std::to_string(deaths) + " times", window.width / 2, 280);
		}
		if(state[2]) {
			window.text("and took " + std::to_string(time) + " seconds", window.width / 2, 380);
		}
		if(state[3]) {
			backButton.update(mouseX, mouseY, mouseClick);
			backButton.draw();
			window.fill(0, 0, 0);
		}
	}
};

//For win fade out, too lazy to move it somewhere better ¯\_(ツ)_/¯
double winBackgroundAlpha = 0;

//Don't know a better place for this one either ¯\_(ツ)_/¯ I think my next game should definetly have one global game state object...
WinInfo winInfo;

//Play footstep sound when player is moving + on ground, reset if otherwise
class Player {
	//Sound variables
	const int stepInterval = 18;
	int stepTimer = stepInterval;
	
public:
	//Player variables
	double x;
	double y;
	
	double prevX;
	double prevY;
	
	int width = 20;
	int height = 32;
	
	double xVel = 0;
	double yVel = 0;
	
	//Animation variables
	enum class Animation {left, right, rightRun, leftRun};
	
	AnimationController<Animation> controller;
	
	//Variable jump height variables
	const double jumpHeight = -1.5;
	double jumpForce = jumpHeight;
	const double jumpDampening = 0.8;
	
	bool jumping = false;
	
	//Jumping variable
	bool canJump = false;
	
	//Spike variables
	double lastX;
	double lastY;
	
	//Locked block + key variables
	int keyAmount = 0;
	bool unlocked = false;
	
	//Deaths :(
	int deaths = 0;
	
	//Have you won??????
	bool win = false;
	
	Player(int x, int y) : controller(images.at("player"), 32, 32, {{Animation::right, {0, 8, 0, 8}}, {Animation::left, {1, 8, 1, 8}}, {Animation::rightRun, {0, 0, 3, 3}}, {Animation::leftRun, {0, 4, 3, 7}}}, Animation::right, 15) {
		this->x = x;
		this->y = y;
		
		prevX = x;
		prevY = y;
		
		lastX = x;
		lastY = y;
	}
	
	void setPosition(double x, double y) {
		this->x = x;
		this->y = y;
		
		//Also set prevX and prevY to avoid weird interpolation
		prevX = x;
		prevY = y;
	}
	
	void die() {
		setPosition(lastX, lastY);
		
		deaths ++;
		
		soloud.play(*audio["death"]);
	}
	
	void update(json &worldMap, std::vector<Block*> &blocks, int &areaX, int &areaY) {
		if(win) {
			winBackgroundAlpha += 0.004;
			
			if(winBackgroundAlpha >= 1) {
				scene = Scene::win;
				
				winInfo.endTimer();
			}
		}
		
		prevX = x;
		prevY = y;
		
		bool left = keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT];
		bool right = keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT];
		
		if((left && right) || (!left && !right)) {
			xVel = 0;
		}else {
			if(left) {
				xVel = -2;
				
				controller.changeState(Animation::leftRun);
			}
			if(right) {
				xVel = 2;
				
				controller.changeState(Animation::rightRun);
			}
			
			if(canJump) {
				stepTimer --;
				
				if(stepTimer <= 0) {
					soloud.play(*audio["step"]);
					
					stepTimer = stepInterval;
				}
			}
		}
		
		if(keys[GLFW_KEY_SPACE]) {
			if(canJump) {
				jumping = true;
				
				jumpForce = jumpHeight;
				
				soloud.play(*audio["jump"]);
			}
			
			if(jumping) {
				yVel += jumpForce;
				
				jumpForce *= jumpDampening;
			}
		}else {
			jumping = false;
		}
		
		if(!xVel) {
			if(controller.state == Animation::leftRun) {
				controller.changeState(Animation::left);
			}else if(controller.state == Animation::rightRun) {
				controller.changeState(Animation::right);
			}
		}
		
		canJump = false;
		
		std::vector<Block*> collisions;
		
		x += xVel;
		
		for(Block* block : blocks) {
			if(collide(x, y, width, height, block->x, block->y, block->width, block->height)) {
				if(block->collision) {
					if(x < block->x) {
						x = block->x - width;
					}else {
						x = block->x + block->width;
					}
				}
				
				//Add to collisions
				collisions.push_back(block);
			}
		}
		
		y += yVel;
		
		for(Block* block : blocks) {
			if(collide(x, y, width, height, block->x, block->y, block->width, block->height)) {
				if(block->collision) {
					if(y < block->y) {
						y = block->y - height;
						
						canJump = true;
						
						yVel = 0;
					}else {
						y = block->y + block->height;
						
						yVel *= -0.01;
					}
				}
				
				//Add to collisions only if block is not already in collisions
				if(std::find(collisions.begin(), collisions.end(), block) == collisions.end()) {
					collisions.push_back(block);
				}
			}
		}
		
		std::vector<Block*> removeList;
		
		for(Block* block : collisions) {
			block->onCollide(*this, blocks, removeList);
		}
		
		for(Block* block : removeList) {
			blocks.erase(std::find(blocks.begin(), blocks.end(), block));
		}
		
		unlocked = false;
		
		xVel = 0;
		
		yVel += 0.3;
		
		if(yVel > 6) {
			yVel = 6;
		}
		
		if(x + width > window.width) {
			setPosition(0, y);
			
			lastX = x;
			lastY = y;
			
			moveArea(worldMap, blocks, areaX, areaY, 1, 0);
		}else if(x < 0) {
			setPosition(window.height - width, y);
			
			lastX = x;
			lastY = y;
			
			moveArea(worldMap, blocks, areaX, areaY, -1, 0);
		}
		
		if(y + height > window.height) {
			setPosition(x, 0);
			
			lastX = x;
			lastY = y;
			
			moveArea(worldMap, blocks, areaX, areaY, 0, 1);
		}else if(y < 0) {
			setPosition(x, window.height - height);
			
			lastX = x;
			lastY = y;
			
			moveArea(worldMap, blocks, areaX, areaY, 0, -1);
		}
	}
	
	void draw(double t) {
		controller.drawFrame(window, std::lerp(prevX - 6, x - 6, t), std::lerp(prevY, y, t), width + 12, height);
	}
};

void LockedBlock::onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) {
	if(player.unlocked) {
		return;
	}
	
	if(player.keyAmount > 0) {
		player.keyAmount --;
		
		player.unlocked = true;
		
		std::vector<Block*> adjacentBlocks = {this};
		
		while(adjacentBlocks.size() > 0) {
			for(int i = static_cast<int>(adjacentBlocks.size()) - 1; i >= 0; i --) {
				Block* adjacentBlock = adjacentBlocks[i];
				
				removeList.push_back(adjacentBlock);
				blocks.push_back(createBlock(BlockType::sandBlockWall, adjacentBlock->x, adjacentBlock->y));
				
				for(Block* block : blocks) {
					if(block->type == BlockType::lockedBlock && adjacent(adjacentBlock->x, adjacentBlock->y, adjacentBlock->width, adjacentBlock->height, block->x, block->y, block->width, block->height) && std::find(adjacentBlocks.begin(), adjacentBlocks.end(), block) == adjacentBlocks.end() && std::find(removeList.begin(), removeList.end(), block) == removeList.end()) {
						adjacentBlocks.push_back(block);
					}
				}
				
				adjacentBlocks.erase(adjacentBlocks.begin() + i);
			}
		}
		
		soloud.play(*audio["unlock"]);
	}
}

void Key::onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) {
	player.keyAmount ++;
	
	removeList.push_back(this);
	
	soloud.play(*audio["pickupKey"]);
}

void Spike::onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) {
	if(triangleRectCollide(x + width / 2, y, x, y + height, x + width, y + height, player.x, player.y, player.width, player.height)) {
		player.die();
	}
}

void DownSpike::onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block *> &removeList) {
	if(triangleRectCollide(x, y, x + width, y, x + width / 2, y + height, player.x, player.y, player.width, player.height)) {
		player.die();
	}
}

void Gem::onCollide(Player &player, std::vector<Block*> &blocks, std::vector<Block*> &removeList) {
	//YOU WIN LET'S GOOOOOOOOOOO
	player.win = true;
	
	removeList.push_back(this);
	
	soloud.play(*audio["pickupKey"]);
}	

json worldMap = json::parse(std::ifstream("map.json"));
	
int x = 0;
int y = 0;

Player player(192, 560);

std::vector<Block*> blocks;
	//Buttons
Button playButton(window.width / 2 - 100, window.height / 2, 200, 50, [](double curAm) {
		return curAm + (0.1 - curAm) * 0.1;
	}, [](double curAm) {
		return curAm - curAm * 0.1;
	}, [](float x, float y, float width, float height, double animTimer) {
		window.save();
		
		window.translate(x - animTimer * width / 2, y - animTimer * height / 2);
		window.scale(animTimer + 1, animTimer + 1);
		
		window.fill(0, 0, 0);
		window.fillRect(0, 0, width, height);
		
		window.fill(255, 255, 255);
		window.textSize = 20;
		window.textAlign = Window2D::TextAlign::center;
		window.text("Play", width / 2, 30);
		
		window.restore();
	}, []() {
		soloud.play(*audio["button"]);
		
		winInfo.startTimer();
		
		scene = Scene::game;
	});
	
	Button backButton(window.width / 2 - 100, 580, 200, 50, [](double curAm) {
		return curAm + (0.1 - curAm) * 0.1;
	}, [](double curAm) {
		return curAm - curAm * 0.1;
	}, [](float x, float y, float width, float height, double animTimer) {
		window.save();
		
		window.translate(x - animTimer * width / 2, y - animTimer * height / 2);
		window.scale(animTimer + 1, animTimer + 1);
		
		window.fill(0, 0, 0);
		window.fillRect(0, 0, width, height);
		
		window.fill(255, 255, 255);
		window.textSize = 20;
		window.textAlign = Window2D::TextAlign::center;
		window.text("Back", width / 2, 30);
		
		window.restore();
	}, []() {
		soloud.play(*audio["button"]);
		
		worldMap = json::parse(std::ifstream("map.json"));
		
		player.win = false;
		
		winBackgroundAlpha = 0;
		
		player.setPosition(192, 560);
		
		x = 0;
		y = 0;
		
		loadBlocks(worldMap, blocks, "0", "0", {});
		
		player.controller.changeState(Player::Animation::rightRun);
		
		winInfo = WinInfo();
		
		scene = Scene::menu;
	});
	//Timestep
	const double framerate = 1.0 / 60.0;
	double curTime;
	
	double accumulator = 0;
void loop() {
		window.getMousePos(mouseX, mouseY);
		
		double newTime = glfwGetTime();
		double frameTime = newTime - curTime;
		curTime = newTime;
		
		accumulator += frameTime;
		
		//Update
		while(accumulator >= framerate) {
			switch(scene) {
				case Scene::menu: {
					playButton.update(mouseX, mouseY, mouseClick);
					
					break;
				}
				case Scene::game: {
					player.update(worldMap, blocks, x, y);
					
					break;
				}
				case Scene::win: {
					winInfo.update();
					
					break;
				}
			}
			
			accumulator -= framerate;
		}
		
		//Draw
		switch(scene) {
			case Scene::menu: {
				//Background
				window.fill(255, 255, 255);
				window.fillRect(0, 0, window.width, window.height);
				
				window.fill(0, 0, 0);
				window.textSize = 50;
				window.textAlign = Window2D::TextAlign::center;
				
				window.text("Find the gem!", window.width / 2, 100);
				
				player.controller.drawFrame(window, window.width / 2 - 160, window.height / 2 - 220, 160, 160);
				
				window.drawImage(images.at("gem"), window.width / 2 + 40, window.height / 2 - 148 + std::sin(curTime * 3) * 8, 80, 80);
				
				playButton.draw();
				
				break;
			}
			case Scene::game: {
				std::string strX = std::to_string(x);
				std::string strY = std::to_string(y);
				
				window.fill(worldMap[strY][strX]["bg"][0], worldMap[strY][strX]["bg"][1], worldMap[strY][strX]["bg"][2]);
				window.fillRect(0, 0, window.width, window.height);
				
				for(Block* block : blocks) {
					block->draw();
				}
				
				player.draw(accumulator / framerate);
				
				//Win fade out
				window.globalAlpha = winBackgroundAlpha;
				
				window.fill(255, 255, 255);
				
				window.fillRect(0, 0, window.width, window.height);
				
				window.globalAlpha = 1;
				
				break;
			}
			case Scene::win: {
				window.fill(255, 255, 255);
				
				window.fillRect(0, 0, window.width, window.height);
				
				winInfo.draw(player.deaths, backButton);
				
				break;
			}
		}
		
		mousePress = false;
		mouseClick = false;
		
		window.render();
		window.update();
}

int main() {
	window.setMouseButtonCallback(mouseButtonCallback);
	window.setKeyCallback(keyCallback);
	
	soloud.init();
	
		
	loadBlocks(worldMap, blocks, "0", "0", {});
	
		
	player.controller.changeState(Player::Animation::rightRun);
	
	window.font = Font(window.ft, "fonts/PixeloidSans.ttf");
	
curTime = glfwGetTime();

	emscripten_set_main_loop(loop, 0, true);

/*	
	
*/
	
	for(Block* block : blocks) {
		delete block;
	}
	
	window.terminate();
	
	return 0;
}
