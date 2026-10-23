#ifndef openGl2D_h
#define openGl2D_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <cmath>
#include <array>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>

using namespace std;

struct Image {
public:
	unsigned char* texture;
	GLfloat width;
	GLfloat height;
	
	Image(const char* filename);
};

struct Primitive {
public:
	GLenum type;
	
	vector<GLfloat> verticies;
	
	int dataPerVertex;
	
	bool textured;
	
	unsigned char* texture;
	
	GLsizei textureWidth;
	GLsizei textureHeight;
	
	Primitive(GLenum type, GLfloat verticies[], int verticiesLength, int numbersPerVertex);
	Primitive(GLenum type, GLfloat verticies[], int verticiesLength, int numbersPerVertex, unsigned char* texture, GLsizei textureWidth, GLsizei textureHeight);
};

class OpenGl2D {
private:
	bool error;
	
	GLfloat fillColor[3] = {0.0f, 0.0f, 0.0f};
	
	GLfloat translateX;
	GLfloat translateY;
	
	GLuint coloredSizeUniformLoc;
	GLuint texturedSizeUniformLoc;
	GLuint imageUniformLoc;
	
	GLint pixelWidth;
	GLint pixelHeight;
	
	GLFWwindow* window;
	
	GLuint coloredProgram;
	GLuint texturedProgram;
	
	GLuint VAO;
	GLuint VBO;
	
	vector<Primitive> primitives;
	
	GLuint texture;
	
public:
	GLint width;
	GLint height;
	
	GLfloat globalAlpha = 1.0f;
	
	OpenGl2D(const char* name, bool maximized, int w = 1000, int h = 1000);
	
	void setKeyCallback(GLFWkeyfun keyCallback);
	
	void setMouseMoveCallback(GLFWcursorposfun mouseMoveCallback);
	
	void setMouseButtonCallback(GLFWmousebuttonfun mouseButtonCallback);
	
	bool isRunning();
	
	double getTime();
	
	void setTime(double time);
	
	void background(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	
	void setFillColor(GLfloat r, GLfloat g, GLfloat b);
	
	void translate(GLfloat x, GLfloat y);
	
	void resetTranslation();
	
	void rect(GLfloat x, GLfloat y, GLfloat w, GLfloat h);
	
	void circle(GLfloat x, GLfloat y, GLfloat rad);
	
	void drawImage(const Image &image, GLfloat x, GLfloat y);
	
	void drawImage(const Image &image, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
	
	void text(string text, GLfloat x, GLfloat y);
	
	void render();
	
	void terminate();
};

#endif
