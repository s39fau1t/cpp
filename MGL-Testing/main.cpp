#include <array>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glcorearb.h>
#include <stb/stb_image.h>

#include "utils.h"

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 800, "Hello, MGL!", nullptr, nullptr);

	glfwMakeContextCurrent(window);
/*
	GLuint program = createProgram("assets/shaders/colored.vert", "assets/shaders/colored.frag");

	std::array vertices = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		0.5f,  0.5f, 0.0f, 0.5f, 1.0f,
	};

	std::array indices = {
		0, 1, 2,
		1, 2, 3
	};
	GLuint VAO = createVertexArray();

	GLuint VBO = createBuffer(vertices);
	GLuint EBO = createBuffer(indices);

	specifyAttrib(VAO, 0, 0, 2);
	specifyAttrib(VAO, 1, 0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);
*/

	GLuint program = createProgram("assets/shaders/textured.vert", "assets/shaders/textured.frag");

	std::array vertices = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 1.0f,
		0.5f,  0.5f, 1.0f, 1.0f
	};

	std::array indices = {
		0, 1, 2,
		1, 2, 3
	};

	GLuint VAO = createVertexArray();

	GLuint VBO = createBuffer(vertices);
	GLuint EBO = createBuffer(indices);

	specifyAttrib(VAO, 0, 0, 2, GL_FLOAT, GL_FALSE, 0);
	specifyAttrib(VAO, 1, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 4 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);

	int width;
	int height;
	int colorChannels;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* image = stbi_load("assets/sprites/hamster.png", &width, &height, &colorChannels, 0);

	GLuint texture = createTexture(GL_TEXTURE_2D, {
		{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
		{GL_TEXTURE_MAG_FILTER, GL_LINEAR},
		{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
		{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
	}, 1, GL_RGBA8, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image, true);

	stbi_image_free(image);

	glViewport(0, 0, 1600, 1600);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		//glBindTextureUnit(GL_TEXTURE0, texture);
		//glUniform1i(glGetUniformLocation(program, "image"), 0);
		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
