#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <iostream>
#include <deque>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <json/json.h>
#include <nfd/nfd.h>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

#include "window2D.h"
#include "animation.h"
#include "loadImages.h"
#include "loadAudio.h"

using json = nlohmann::json;

const int areaWidth = 800;
const int areaHeight = 800;

Window2D window(800, 819, "Level Editor", false);

enum class Scene {menu, editor, win};

Scene scene = Scene::menu;

//TODO: Make it so that it detects file name instead of having to specify it


std::map<std::string, Image> images = loadImages({{"player", "assets/images/player.png"}, {"sandBlock", "assets/images/sand_block.png"}, {"lockedBlock", "assets/images/locked_block.png"}, {"key", "assets/images/key.png"}, {"sandBlockWall", "assets/images/sand_block_wall.png"}, {"spike", "assets/images/spike.png"}, {"downSpike", "assets/images/down_spike.png"}, {"gem", "assets/images/gem.png"}, {"barrier", "assets/images/barrier.png"}});

//soloud library object
SoLoud::Soloud soloud;

//I have to allocate these on the heap because they "are not meant to be copied." https://github.com/jarikomppa/soloud/issues/283 SERIOUSLY HOW HARD IS IT TO ADD A COPY CONSTRUCTOR
std::map<std::string, SoLoud::Wav*> audio = loadAudio({{"step", "assets/audio/step.wav"}, {"jump", "assets/audio/jump.wav"}, {"pickupKey", "assets/audio/pickup_key.wav"}, {"unlock", "assets/audio/unlock.wav"}, {"death", "assets/audio/death.wav"}});

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
	virtual void onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) {};
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
	
	void onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class Key : public Block {
public:
	Key(int x, int y) : Block(x, y, BlockType::key, false, 16, 7) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("key"), x, y, width, height);
	}
	
	void onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) override;
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
	
	void onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class DownSpike : public Block {
public:
	DownSpike(int x, int y) : Block(x, y, BlockType::downSpike, false) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("downSpike"), x, y, width, height);
	}
	
	void onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class Gem : public Block {
public:
	Gem(int x, int y) : Block(x, y, BlockType::gem, false) {
		
	}
	
	void draw() override {
		window.drawImage(images.at("gem"), x, y, width, height);
	}
	
	void onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) override;
};

class Barrier : public Block {
public:
	Barrier(int x, int y) : Block(x, y, BlockType::barrier) {
		
	}
	
	// !!!: Don't use barrier image in final game, just for ease of editing
	void draw() override {
		window.drawImage(images.at("barrier"), x, y, width, height);
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

void addToBlocks(std::deque<Block*> &blocks, Block* block) {
	switch(block->type) {
		case BlockType::spike:
		case BlockType::downSpike:
		case BlockType::sandBlockWall:
			blocks.push_front(block);
			break;
		default:
			blocks.push_back(block);
			break;
	}
}

void loadBlocks(json &worldMap, std::deque<Block*> &blocks, std::string_view x, std::string_view y, float bg[3]) {
	for(Block* block : blocks) {
		delete block;
	}
	
	blocks.clear();
	
	if(worldMap.find(y) == worldMap.end()) {
		worldMap[y] = json::object();
	}
	
	if(worldMap[y].find(x) == worldMap[y].end()) {
		worldMap[y][x] = {
			{"bg", {bg[0], bg[1], bg[2]}},
			{"blocks", {}}
		};
	}
	
	for(json::object_t block : worldMap[y][x]["blocks"]) {
		addToBlocks(blocks, createBlock(stringToBlockType(block["type"]), block["x"], block["y"]));
	}
}

void addToMap(json &worldMap, const std::deque<Block*> &blocks, std::string_view x, std::string_view y) {
	worldMap[y][x]["blocks"].clear();
	
	for(Block* block : blocks) {
		worldMap[y][x]["blocks"].push_back({
			{"x", block->x},
			{"y", block->y},
			{"type", blockTypeToString(block->type)},
		});
	}
}

void moveArea(json &worldMap, std::deque<Block*> &blocks, bool &saved, bool deleteMode, int &x, int &y, int movX, int movY) {
	std::string strX = std::to_string(x);
	std::string strY = std::to_string(y);
	
	float bg[3] = {worldMap[strY][strX]["bg"][0], worldMap[strY][strX]["bg"][1], worldMap[strY][strX]["bg"][2]};
	
	if(deleteMode) {
		worldMap[strY].erase(strX);
		
		if(worldMap[strY].empty()) {
			worldMap.erase(strY);
		}
	}else {
		addToMap(worldMap, blocks, strX, strY);
	}
	
	x += movX;
	y += movY;
	
	strX = std::to_string(x);
	strY = std::to_string(y);
	
	loadBlocks(worldMap, blocks, strX, strY, bg);
	
	saved = false;
}

class WinInfo {
	int t = 0;
	
	double start;
	double time;
	
	std::vector<int> times = {150, 300, 450, 600};
	
	std::vector<bool> state;
	
public:
	WinInfo() {
		start = glfwGetTime();
		
		for(int i = 0; i < times.size(); i ++) {
			state.push_back(false);
		}
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
	
	void draw(int deaths) {
		window.fill(0, 0, 0);
		
		window.textAlign = Window2D::TextAlign::center;
		
		if(state[0]) {
			window.textSize = 50;
			
			window.text("You Win", window.width / 2, 80);
		}
		if(state[1]) {
			window.textSize = 30;
			
			window.text("You died " + std::to_string(deaths) + " times!", window.width / 2, 280);
		}
		if(state[2]) {
			window.text("You took " + std::to_string(time) + " seconds!", window.width / 2, 380);
		}
		if(state[3]) {
			
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
	
	//Animation variables
	enum class Animation {left, right, rightRun, leftRun};
	
	AnimationController<Animation> controller;
	
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
	int keys = 0;
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
	
	void update(json &worldMap, std::deque<Block*> &blocks, bool &saved, bool deleteMode, int &areaX, int &areaY) {
		if(win) {
			winBackgroundAlpha += 0.004;
			
			if(winBackgroundAlpha >= 1) {
				scene = Scene::win;
				
				winInfo.endTimer();
			}
		}
		
		prevX = x;
		prevY = y;
		
		bool left = ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_LeftArrow);
		bool right = ImGui::IsKeyDown(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_RightArrow);
		
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
		
		if(ImGui::IsKeyDown(ImGuiKey_Space)) {
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
		
		if(x + width > areaWidth) {
			setPosition(0, y);
			
			lastX = x;
			lastY = y;
			
			moveArea(worldMap, blocks, saved, deleteMode, areaX, areaY, 1, 0);
		}else if(x < 0) {
			setPosition(areaWidth - width, y);
			
			lastX = x;
			lastY = y;
			
			moveArea(worldMap, blocks, saved, deleteMode, areaX, areaY, -1, 0);
		}
		
		if(y + height > areaHeight) {
			setPosition(x, 0);
			
			lastX = x;
			lastY = y;
			
			moveArea(worldMap, blocks, saved, deleteMode, areaX, areaY, 0, 1);
		}else if(y < 0) {
			setPosition(x, areaHeight - height);
			
			lastX = x;
			lastY = y;
			
			moveArea(worldMap, blocks, saved, deleteMode, areaX, areaY, 0, -1);
		}
	}
	
	void draw(double t) {
		controller.drawFrame(window, std::lerp(prevX - 6, x - 6, t), std::lerp(prevY, y, t), width + 12, height);
	}
};

void LockedBlock::onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) {
	if(player.unlocked) {
		return;
	}
	
	if(player.keys > 0) {
		player.keys --;
		
		player.unlocked = true;
		
		std::vector<Block*> adjacentBlocks = {this};
		
		while(adjacentBlocks.size() > 0) {
			for(int i = static_cast<int>(adjacentBlocks.size()) - 1; i >= 0; i --) {
				Block* adjacentBlock = adjacentBlocks[i];
				
				removeList.push_back(adjacentBlock);
				addToBlocks(blocks, createBlock(BlockType::sandBlockWall, adjacentBlock->x, adjacentBlock->y));
				
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

void Key::onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) {
	player.keys ++;
	
	removeList.push_back(this);
	
	soloud.play(*audio["pickupKey"]);
}

void Spike::onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) {
	if(triangleRectCollide(x + width / 2, y, x, y + height, x + width, y + height, player.x, player.y, player.width, player.height)) {
		player.die();
	}
}

void DownSpike::onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block *> &removeList) {
	if(triangleRectCollide(x, y, x + width, y, x + width / 2, y + height, player.x, player.y, player.width, player.height)) {
		player.die();
	}
}

void Gem::onCollide(Player &player, std::deque<Block*> &blocks, std::vector<Block*> &removeList) {
	//YOU WIN LET'S GOOOOOOOOOOO
	player.win = true;
	
	removeList.push_back(this);
	
	soloud.play(*audio["pickupKey"]);
}

//https://github.com/ocornut/imgui/discussions/3862
bool ButtonCenteredOnLine(const char* label, const ImVec2 &buttonSize = {0, 0}, float alignment = 0.5f) {
	ImGuiStyle& style = ImGui::GetStyle();

	float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if (off > 0.0f) {
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
	}

	return ImGui::Button(label, buttonSize);
}

void TextCenteredOnLine(const char* text, float alignment = 0.5f) {
	ImGuiStyle& style = ImGui::GetStyle();

	float size = ImGui::CalcTextSize(text).x + style.FramePadding.x * 2.0f;
	float avail = ImGui::GetContentRegionAvail().x;

	float off = (avail - size) * alignment;
	if(off > 0.0f) {
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
	}
	
	ImGui::Text("%s", text);
}

bool ImageButton(const Image &image, ImVec2 size) {
	bool clicked = ImGui::ImageButton(reinterpret_cast<ImTextureID>(image.texture), size, ImVec2(0, 1), ImVec2(1, 0));
	
	return clicked;
}

void saveFile(nfdchar_t* &filePath, bool &saved, json &worldMap, const std::deque<Block*> &blocks, std::string_view x, std::string_view y) {
	if(filePath == nullptr) {
		nfdresult_t result = NFD_SaveDialog("json", nullptr, &filePath);
		
		if(result != NFD_OKAY) {
			return;
		}
	}
	
	addToMap(worldMap, blocks, x, y);
	
	std::ofstream file(filePath);
	
	file << worldMap.dump();
	
	saved = true;
}

int main() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	
	soloud.init();
	
	json worldMap;

	nfdchar_t* filePath = nullptr;

	std::string filename;

	bool saved = false;

	std::deque<Block*> blocks;

	int x = 0;
	int y = 0;
	
	BlockType curBlockType = BlockType::sandBlock;
	
	bool showControls = false;
	
	bool playing = false;
	
	bool deleteMode = false;
	
	Player player(0, 0);
	
	const double framerate = 1.0 / 60.0;
	double curTime = glfwGetTime();
	
	double accumulator = 0;
	
	while(!window.shouldClose()) {
		double newTime = glfwGetTime();
		double frameTime = newTime - curTime;
		curTime = newTime;
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		switch(scene) {
			case Scene::menu: {
				window.fill(18, 33, 43);
				window.fillRect(0, 0, window.width, window.height);
				
				ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
				ImGui::SetWindowSize(ImVec2(300, 250), ImGuiCond_Once);
				ImGui::SetWindowPos(ImVec2(viewport->Size.x / 2.0f - 150, viewport->Size.y / 2.0f - 150));
				TextCenteredOnLine("Welcome");
				ImGui::NewLine();
				if(ButtonCenteredOnLine("Open existing map file", ImVec2(0, 80))) {
					nfdresult_t result = NFD_OpenDialog("json", nullptr, &filePath);
					
					if(result == NFD_OKAY) {
						std::ifstream file(filePath);
						
						worldMap = json::parse(file);
						
						loadBlocks(worldMap, blocks, "0", "0", {});
						
						filename = filePath;
						filename = filename.substr(filename.find_last_of("/") + 1);
						
						saved = true;
						
						scene = Scene::editor;
					}
				}
				ImGui::NewLine();
				if(ButtonCenteredOnLine("Create new map file", ImVec2(0, 80))) {
					worldMap = {
						{
							"0", {
								{
									"0", {
										{
											"bg", {18.0f, 33.0f, 43.0f}
										},
										{
											"blocks", {}
										}
									}
								}
							}
						}
					};
					
					filename = "Untitled.json";
					
					scene = Scene::editor;
				}
				ImGui::End();
				break;
			}
			case Scene::editor: {
				window.save();
				
				std::string strX = std::to_string(x);
				std::string strY = std::to_string(y);
				
				window.fill(worldMap[strY][strX]["bg"][0], worldMap[strY][strX]["bg"][1], worldMap[strY][strX]["bg"][2]);
				window.fillRect(0, 0, window.width, window.height);
				
				window.translate(0, 19);
				
				for(Block* block : blocks) {
					block->draw();
				}
				
				if(!io.WantTextInput) {
					//Movement code
					if(ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
						moveArea(worldMap, blocks, saved, deleteMode, x, y, -1, 0);
					}else if(ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
						moveArea(worldMap, blocks, saved, deleteMode, x, y, 1, 0);
					}else if(ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
						moveArea(worldMap, blocks, saved, deleteMode, x, y, 0, -1);
					}else if(ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
						moveArea(worldMap, blocks, saved, deleteMode, x, y, 0, 1);
					}
					
					if(ImGui::IsKeyPressed(ImGuiKey_P)) {
						playing = !playing;
						
						if(playing) {
							ImVec2 mousePos = ImGui::GetMousePos();
							
							player.setPosition(mousePos.x, mousePos.y);
						}
					}
					
					if(playing) {
						accumulator += frameTime;
						
						while(accumulator >= framerate) {
							player.update(worldMap, blocks, saved, deleteMode, x, y);
							
							accumulator -= framerate;
						}
					}
				}
				
				if(playing) {
					player.draw(accumulator / framerate);
				}
				
				if(!io.WantCaptureMouse) {
					ImVec2 mousePos = ImGui::GetMousePos();
					
					mousePos.y -= 19;
					
					Block* curBlock = createBlock(curBlockType, mousePos.x, mousePos.y);
					
					if(ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) {
						int closestX = 0;
						int closestY = 0;
						
						double closestDist = std::numeric_limits<double>::infinity();
						
						for(Block* block : blocks) {
							int leftX = block->x;
							int leftY = block->y + block->height / 2.0;
							
							int rightX = block->x + block->width;
							int rightY = block->y + block->height / 2.0;
							
							int upX = block->x + block->width / 2.0;
							int upY = block->y;
							
							int downX = block->x + block->width / 2.0;
							int downY = block->y + block->height;
							
							double leftDist = dist(mousePos.x, mousePos.y, leftX, leftY);
							double rightDist = dist(mousePos.x, mousePos.y, rightX, rightY);
							double upDist = dist(mousePos.x, mousePos.y, upX, upY);
							double downDist = dist(mousePos.x, mousePos.y, downX, downY);
							
							if(leftDist < closestDist) {
								closestDist = leftDist;
								
								closestX = leftX - curBlock->width / 2.0;
								closestY = leftY;
							}
							if(rightDist < closestDist) {
								closestDist = rightDist;
								
								closestX = rightX + curBlock->width / 2.0;
								closestY = rightY;
							}
							if(upDist < closestDist) {
								closestDist = upDist;
								
								closestX = upX;
								closestY = upY - curBlock->height / 2.0;
							}
							if(downDist < closestDist) {
								closestDist = downDist;
								
								closestX = downX;
								closestY = downY + curBlock->height / 2.0;
							}
						}
						
						if(closestDist < 20) {
							mousePos.x = closestX;
							mousePos.y = closestY;
						}
					}else if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
						//TODO: Maybe there is a cleaner way to do this besides relying on integer division?
						ImVec2 middle(curBlock->width / 2.0f, curBlock->height / 2.0f);
						
						mousePos.x = static_cast<int>(mousePos.x + middle.x) / curBlock->width * curBlock->width - middle.x;
						mousePos.y = static_cast<int>(mousePos.y + middle.y) / curBlock->height * curBlock->height - middle.y;
					}
						
					curBlock->x = mousePos.x;
					curBlock->y = mousePos.y;
					
					int width = curBlock->width;
					int height = curBlock->height;
					
					curBlock->x -= width / 2.0;
					curBlock->y -= height / 2.0;
					
					curBlock->draw();
					
					if(mousePos.x >= 0 && mousePos.x < 800 && mousePos.y >= 0 && mousePos.y < 800) {
						bool mouse;
						
						if(ImGui::IsKeyDown(ImGuiKey_LeftSuper) || ImGui::IsKeyDown(ImGuiKey_RightSuper)) {
							mouse = ImGui::IsMouseDown(ImGuiMouseButton_Left);
						}else {
							mouse = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
						}
						
						if(mouse) {
							std::deque<Block*>::iterator it = std::find_if(blocks.begin(), blocks.end(), [&](Block* block) {
								return block->x == mousePos.x - width / 2.0 && block->y == mousePos.y - width / 2.0;
							});
							
							if(it != blocks.end() && (*it)->type != BlockType::sandBlockWall) {
								delete *it;
								
								blocks.erase(it);
							}
							
							addToBlocks(blocks, curBlock);
							
							saved = false;
						}else {
							delete curBlock;
						}
					}
					
					if(ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
						for(int i = 0; i < blocks.size(); i ++) {
							Block* block = blocks[i];
							
							if(collide(mousePos.x - width / 2.0, mousePos.y - height / 2.0, width, height, block->x, block->y, block->width, block->height)) {
								blocks.erase(blocks.begin() + i);
								
								delete block;
							}
						}
					}
				}
				
				window.globalAlpha = winBackgroundAlpha;
				
				window.fill(255, 255, 255);
				
				window.fillRect(0, 0, window.width, window.height);
				
				window.globalAlpha = 1;
				
				window.restore();
				
				if(ImGui::BeginMainMenuBar()) {
					if(ImGui::BeginMenu("File")) {
						if(ImGui::MenuItem("Save")) {
							saveFile(filePath, saved, worldMap, blocks, strX, strY);
						}
						
						ImGui::EndMenu();
					}
					
					if(ImGui::BeginMenu("Area")) {
						ImGui::MenuItem("Delete Mode", NULL, &deleteMode);
						
						if(ImGui::BeginMenu("Background")) {
							float bg[3] = {static_cast<float>(worldMap[strY][strX]["bg"][0]) / 255, static_cast<float>(worldMap[strY][strX]["bg"][1]) / 255, static_cast<float>(worldMap[strY][strX]["bg"][2]) / 255};

							ImGui::ColorPicker3("", bg);

							worldMap[strY][strX]["bg"][0] = bg[0] * 255;
							worldMap[strY][strX]["bg"][1] = bg[1] * 255;
							worldMap[strY][strX]["bg"][2] = bg[2] * 255;

							ImGui::EndMenu();
						}
						
						ImGui::EndMenu();
					}
					
					if(ImGui::BeginMenu("Help")) {
						ImGui::MenuItem("Controls", "", &showControls);
						
						ImGui::EndMenu();
					}
					
					TextCenteredOnLine((filename + (saved ? "" : "*") + " " + std::to_string(x) + ", " + std::to_string(y)).c_str());
					ImGui::EndMainMenuBar();
				}
				
				if(ImGui::Begin("Blocks", nullptr, ImGuiWindowFlags_NoResize)) {
					ImGui::SetWindowCollapsed(false, ImGuiCond_Once);
					ImGui::SetWindowSize(ImVec2(75, 500), ImGuiCond_Once);
					ImGui::SetWindowPos(ImVec2(viewport->Size.x - 75, viewport->WorkPos.y), ImGuiCond_Once);
					
					if(ImageButton(images.at("sandBlock"), ImVec2(50, 50))) {
						curBlockType = BlockType::sandBlock;
					}
					if(ImageButton(images.at("lockedBlock"), ImVec2(50, 50))) {
						curBlockType = BlockType::lockedBlock;
					}
					if(ImageButton(images.at("key"), ImVec2(50, 21.875))) {
						curBlockType = BlockType::key;
					}
					if(ImageButton(images.at("sandBlockWall"), ImVec2(50, 50))) {
						curBlockType = BlockType::sandBlockWall;
					}
					if(ImageButton(images.at("spike"), ImVec2(50, 50))) {
						curBlockType = BlockType::spike;
					}
					if(ImageButton(images.at("downSpike"), ImVec2(50, 50))) {
						curBlockType = BlockType::downSpike;
					}
					if(ImageButton(images.at("gem"), ImVec2(50, 50))) {
						curBlockType = BlockType::gem;
					}
					if(ImageButton(images.at("barrier"), ImVec2(50, 50))) {
						curBlockType = BlockType::barrier;
					}
				}
				ImGui::End();
				
				if(showControls) {
					if(ImGui::Begin("Controls", &showControls)) {
						ImGui::Text("Left Click - Place block\nRight Click - Remove blocks\nShift - Snap to nearby blocks\nCtrl - Snap to grid\nCmd - Continuously place blocks\nArrow keys - Change areas\nP - Add/remove player\n\nPlayer controls\nA, D, spacebar - Move");
					}
					ImGui::End();
				}
				break;
			}
			case Scene::win: {
				window.fill(255, 255, 255);
				
				window.fillRect(0, 0, window.width, window.height);
				
				winInfo.update();
				winInfo.draw(player.deaths);
				
				break;
			}
		}
		
		window.render();
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		window.update();
	}
	
	//Free heap-allocated audio (I HATE YOU SOLOUD JUST ADD A COPY CONSTRUCTOR I'M SO MAD RIGHT NOW DO YOU SEE HOW MAD I AM AAAAAAAAAAAAAAAAAAAAAAAAAAAA)
	for(auto &entry : audio) {
		delete entry.second;
	}
	
	//Terminate all our libraries
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	
	ImGui::DestroyContext();
	
	window.terminate();
	
	soloud.deinit();
	
	return 0;
}
