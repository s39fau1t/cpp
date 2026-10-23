#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>

int key = 0;
std::vector<int> keyQueue = {};

void updateKeys(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key != GLFW_KEY_UNKNOWN) {
		if(action == GLFW_PRESS) {
			if(::key == 0) {
				::key = key;
			}else if(keyQueue.size() < 4) {
				keyQueue.push_back(key);
			}
		}
	}
}

/*
bool keys[348];

void updateKeys(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key != GLFW_KEY_UNKNOWN) {
		if(action == GLFW_PRESS) {
			keys[key] = true;
		}
	}
}
 */

class OpenGl2D {
private:
	bool error = false;
	
	GLFWwindow* window;
	GLuint shaderProgram;
	GLuint VAO;
	GLuint VBO;
	std::vector<GLfloat> verticies;
	
public:
	OpenGl2D() {
		//Initialize glfw
		glfwInit();

		//Version 3.3, foreward compatible for mac, and only new functions
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		//Window
		window = glfwCreateWindow(1000, 1000, "Snake", NULL, NULL);

		//Error with the window ;-;
		if(window == NULL) {
			std::cout << "Failed to create a GLFW window\n";
			glfwTerminate();
			error = true;
		}

		//Tell glfw we're using the window
		glfwMakeContextCurrent(window);
		
		glfwSetKeyCallback(window, updateKeys);

		//Load the correct functions with glad
		gladLoadGL();

		//Set the viewport
		//glViewport(0, 0, 800, 800);
		
		//Vertex shader
		const char* vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"void main() {\n"
		"	vec2 zeroToOne = aPos / 1000.0;\n"
		"	vec2 zeroToTwo = zeroToOne * 2.0;\n"
		"	vec2 clipSpace = zeroToTwo - 1.0;\n"
		"	\n"
		"	gl_Position = vec4(clipSpace.x, clipSpace.y * -1.0, 0.0, 1.0);\n"
		"}";

		//Fragment shader
		const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
		"}";
		
		//Create and compile the vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		
		//And the fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		
		//The program to hold both of the shaders
		shaderProgram = glCreateProgram();
		
		//Attach them to the program
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		
		//Link the program
		glLinkProgram(shaderProgram);
		
		GLchar vertexLog[1024];
		GLchar fragmentLog[1024];
		GLsizei vertexLength;
		GLsizei fragmentLength;
		
		glGetShaderInfoLog(vertexShader, sizeof(vertexLog), &vertexLength, vertexLog);
		glGetShaderInfoLog(fragmentShader, sizeof(fragmentLog), &fragmentLength, fragmentLog);
		
		//We don't need the shaders anymore, delete them
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		
		//Generate the VAO
		glGenVertexArrays(1, &VAO);
		
		//Generate the VBO
		glGenBuffers(1, &VBO);
	}
	
	bool isRunning() {
		if(!glfwWindowShouldClose(window) && !error) {
			return true;
		}else {
			return false;
		}
	}
	
	void background(double r, double g, double b, double a) {
		glClearColor(r / 255, g / 255, b / 255, a);
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		verticies.clear();
	}

	void rect(GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
		GLfloat verticiesToPush[] = {
			x, y,
			x + w, y,
			x, y + h,
			x + w, y,
			x, y + h,
			x + w, y + h
		};
		
		for(GLfloat vertex : verticiesToPush) {
			verticies.push_back(vertex);
		}
	}
	
	void render() {
		//Bind the VAO
		glBindVertexArray(VAO);
		
		//Bind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		
		GLfloat verticiesArray[verticies.size()];
		
		std::copy(verticies.begin(), verticies.end(), verticiesArray);
		
		//Add the verticies to the buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticiesArray), verticiesArray, GL_STATIC_DRAW);
		
		//Put the values of the buffer into the attribute and tell opengl how it is formatted
		//the attribute, the number of values per vertex, the type of data, only matters for integers, the amount of data in between vertexes, and a pointer to where the data beggins in the array
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		//Enable it
		glEnableVertexAttribArray(0);
		
		//Unbind both of them just in case
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		//Use the program with our shaders
		glUseProgram(shaderProgram);
		
		//Bind the vertex array
		glBindVertexArray(VAO);
		
		//Draw the triangles
		glDrawArrays(GL_TRIANGLES, 0, GLsizei(verticies.size() / 2));
		
		//Render
		glfwSwapBuffers(window);
		
		//Poll the window for user input
		glfwPollEvents();
	}
	
	void terminate() {
		//When we're done delete everything, close the window, and terminate glfw
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteProgram(shaderProgram);

		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

OpenGl2D ctx;

bool firstRand = true;

int randNum(int max) {
	if(firstRand) {
		srand(unsigned(time(0)));
		firstRand = false;
	}
	
	return rand() % max;
};

//50

class Player {
private:
	int facing;
	
public:
	int x;
	int y;
	
	std::vector<std::map<std::string, int>> segments;
	
	Player() {
		x = randNum(50);
		y = randNum(50);
		facing = 0;
	}
	
	void draw();
	
	void update();
};

Player player;

class Food {
public:
	int x;
	int y;
	
	Food() {
		std::vector<std::map<std::string, int>> availibleSpaces = {};
		
		for(int y = 0; y < 50; y ++) {
			for(int x = 0; x < 50; x ++) {
				bool isTaken = false;
				for(std::map<std::string, int> segment : player.segments) {
					if(x == segment["x"] && y == segment["y"]) {
						isTaken = true;
						break;
					}
				}
				if(!isTaken) {
					availibleSpaces.push_back({{"x", x}, {"y", y}});
				}
			}
		}
		
		int randInt = randNum(int(availibleSpaces.size()));
		
		Food::x = availibleSpaces[randInt]["x"];
		Food::y = availibleSpaces[randInt]["y"];
	}
	
	void draw() {
		ctx.rect(x * 20, y * 20, 20, 20);
	}
};

Food food;

void Player::draw() {
	if(key == 65 || key == 263) {
		if(segments.size() == 0 || facing != 2) {
			facing = 1;
		}
	}else if(key == 68 || key == 262) {
		if(segments.size() == 0 || facing != 1) {
			facing = 2;
		}
	}else if(key == 87 || key == 265) {
		if(segments.size() == 0 || facing != 4) {
			facing = 3;
		}
	}else if(key == 83 || key == 264) {
		if(segments.size() == 0 || facing != 3) {
			facing = 4;
		}
	}
	
	ctx.rect(x * 20 + (facing > 2 ? 2 : 0), y * 20 + (facing < 3 ? 2 : 0), facing > 2 ? 16 : 20, facing < 3 ? 16 : 20);
	
	for(int i = 0; i < segments.size(); i ++) {
		bool segFace = i != 0 ? segments[i - 1]["x"] == segments[i]["x"] : x == segments[i]["x"];
		
		ctx.rect(segments[i]["x"] * 20 + (segFace ? 2 : 0), segments[i]["y"] * 20 + (!segFace ? 2 : 0), segFace ? 16 : 20, !segFace ? 16 : 20);
	}
}

void Player::update() {
	for(int i = int(segments.size()) - 1; i >= 0; i --) {
		if(i != 0) {
			segments[i] = segments[i - 1];
		}else {
			segments[i]["x"] = x;
			segments[i]["y"] = y;
		}
	}
	 
	switch(facing) {
		case 1:
			x --;
			break;
		case 2:
			x ++;
			break;
		case 3:
			y --;
			break;
		case 4:
			y ++;
			break;
	}
	
	if(x == food.x && y == food.y) {
		if(segments.size() > 1) {
			int xOff = segments[segments.size() - 1]["x"] - segments[segments.size() - 2]["x"];
			int yOff = segments[segments.size() - 1]["y"] - segments[segments.size() - 2]["y"];
			
			segments.push_back({{"x", xOff + segments[segments.size() - 1]["x"]}, {"y", yOff + segments[segments.size() - 1]["y"]}});
		}else {
			if(segments.size() == 0) {
				segments.push_back({{"x", facing < 3 ? facing == 1 ? x + 1 : x - 1 : x}, {"y", facing > 2 ? facing == 3 ? y + 1 : y - 1 : y}});
			}else {
				std::vector<std::map<std::string, int>> headSegs = {{{"x", x}, {"y", y}}};
				
				for(std::map<std::string, int> segment : segments) {
					headSegs.push_back(segment);
				}
				
				int xOff = headSegs[headSegs.size() - 1]["x"] - headSegs[headSegs.size() - 2]["x"];
				int yOff = headSegs[headSegs.size() - 1]["y"] - headSegs[headSegs.size() - 2]["y"];
				
				segments.push_back({{"x", xOff + headSegs[headSegs.size() - 1]["x"]}, {"y", yOff + headSegs[headSegs.size() - 1]["y"]}});
			}
		}
		
		Food newFood;
		
		food = newFood;
	}
	
	if(x < 0 || x > 49 || y < 0 || y > 49) {
		Food startingFood;
		
		food = startingFood;
		
		Player newPlayer;
		
		player = newPlayer;
		
		key = 0;
		keyQueue.clear();
		
		return;
	}
	
	for(std::map<std::string, int> segment : segments) {
		if(x == segment["x"] && y == segment["y"]) {
			Food startingFood;
			
			food = startingFood;
			
			Player newPlayer;
			
			player = newPlayer;
			
			key = 0;
			keyQueue.clear();
			
			break;
		}
	}
}

int main() {
	int prev = int(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count());
	
	int lag = 0;
	
	//Is true until the X is pressed or something else happens that closes the window
	while(ctx.isRunning()) {
		ctx.background(0, 0, 0, 1);
		
		food.draw();
		
		player.draw();
		
		int curr = int(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count());
		int elapsed = curr - prev;
		prev = curr;
		lag += elapsed;
		
		while(lag >= 100) {
			player.update();
			
			if(keyQueue.size() > 0) {
				key = keyQueue[0];
				keyQueue.erase(keyQueue.begin());
			}
			
			lag -= 100;
		}
			
		ctx.render();
	}
	
	ctx.terminate();
	return 0;
}
