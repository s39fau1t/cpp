#ifndef window2D_h
#define window2D_h

#include <iostream>
#include <string>
#include <string_view>
#include <map>
#include <fstream>
#include <sstream>
#include <utility>
#include <cerrno>
#include <vector>
#include <functional>
#include <array>

//#include <glad/glad.h>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Image {
	GLuint texture;
	
	int width;
	int height;
	
	Image(const char* path);
	Image(unsigned char* bytes, int width, int height);
	
	Image(const Image &that);
	Image(Image &&that);
	
	Image &operator=(Image that);
	
	~Image();
};

struct Character {
	GLuint texture;
	
	int width;
	int height;
	
	int bearingX;
	int bearingY;
	
	unsigned int advance;
	
	Character(unsigned char* bytes, int width, int height, int bearingX, int bearingY, unsigned int advance);
};

struct Font {
	std::map<char, Character> characters;
	
	Font(const FT_Library &ft, const char* path);
	Font();
};

struct Attribute {
	GLuint index;
	GLuint size;
	GLenum type;
	GLsizei stride;
	void* offset;
	
	Attribute(GLuint index, GLuint size, GLenum type, GLsizei stride, void* offset);
};

struct Primitive;

struct Program {
	GLuint ID;
	
	std::vector<Attribute> attributes;
	
	std::function<void(const Primitive &)> setUniforms;
	
	Program(const std::string &vertexPath, const std::string &fragmentPath, std::vector<Attribute> attributes, std::function<void(const Primitive &)> setUniforms);
	Program();
};

struct Primitive {
	GLenum type;
	Program program;
	
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	
	glm::mat4 matrix;
	
	std::array<float, 4> color;
	
	GLuint texture;
	
	Primitive(GLenum type, Program program, std::vector<GLfloat> vertices, std::vector<GLuint> indices, glm::mat4 matrix, std::array<float, 4> color, GLuint texture = 0);
};

class Window2D {
public:
	enum class StretchMode {disabled, stretchKeepAspect, stretch};
	
private:

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	
	Program coloredProgram;
	Program texturedProgram;
	Program textProgram;
	
	std::vector<Primitive> primitives;
	
	float fillColor[3] = {0, 0, 0};
	
	StretchMode stretchMode;
	
	glm::mat4 orthoMatrix;
	std::vector<glm::mat4> matrixStack = {glm::identity<glm::mat4>()};
	
	glm::mat4 &getCurrentMatrix();
	void setCurrentMatrix(glm::mat4 matrix);
	
public:
	GLFWwindow* window;
	
	int width;
	int height;
	
	float globalAlpha = 1;
	
	FT_Library ft;
	
	int textSize = 64;
	
	Font font;
	
	enum class TextAlign {left, center, right};
	
	TextAlign textAlign = TextAlign::left;
	
	Window2D(int width, int height, std::string_view title, StretchMode stretchMode = StretchMode::disabled, bool maximized = false, bool resizable = true);
	
	void getMousePos(double &xpos, double &ypos);
	void setMouseButtonCallback(GLFWmousebuttonfun callback);
	void setKeyCallback(GLFWkeyfun callback);
	bool shouldClose();
	void fill(float r, float g, float b);
	void save();
	void translate(float x, float y);
	void rotate(float angle);
	void scale(float x, float y);
	void restore();
	void fillRect(float x, float y, float width, float height);
	void drawImage(const Image &image, float x, float y);
	void drawImage(const Image &image, float x, float y, float width, float height);
	void drawImage(const Image &image, float sX, float sY, float sWidth, float sHeight, float x, float y, float width, float height);
	float getTextWidth(std::string_view text);
	void text(std::string_view text, float x, float y);
	void hideCursor();
	void showCursor();
	void render();
	void update();
	void terminate();
};

#endif
