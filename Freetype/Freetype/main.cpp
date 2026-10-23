#include <map>
#include <string>
#include <iostream>

#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.h"

using namespace std;

struct Character {
	GLuint TextureID;
	glm::ivec2 Size;
	glm::ivec2 Bearing;
	unsigned int Advance;
};

map<char, Character> Characters;

GLuint VAO, VBO;

void RenderText(Shader &s, string text, float x, float y, float scale, glm::vec3 color) {
	s.use();
	
	glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
	
	glActiveTexture(GL_TEXTURE0);
	
	glBindVertexArray(VAO);
	
	for(int i = 0; i < text.size(); i ++) {
		Character ch = Characters[text[i]];
		
		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		x += (ch.Advance >> 6) * scale;
	}
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int main() {
	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 3);
#endif
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Text", NULL, NULL);
	
	glfwMakeContextCurrent(window);
	
	gladLoadGL();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Shader shader("text.vert", "text.frag");
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 800.0f);
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "text"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	FT_Library ft;
	if(FT_Init_FreeType(&ft)) {
		cout << "ERROR:FREETYPE: Could not init FreeType Library\n";
		
		return -1;
	}
	
	FT_Face face;
	if(FT_New_Face(ft, "fonts/Arial.ttf", 0, &face)) {
		cout << "ERROR::FREETYPE: Failed to load font\n";
		
		return -1;
	}
	
	FT_Set_Pixel_Sizes(face, 0, 48);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	for(unsigned char c = 0; c < 128; c ++) {
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			cout << "ERROR::FREETYTPE: Failed to load Glyph\n";
			
			continue;
		}
		
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		Character character = {texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), static_cast<unsigned int>(face->glyph->advance.x)};
		
		Characters.insert(pair<char, Character>(c, character));
	}
	
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		RenderText(shader, "Test", 280.0f, 400.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	
	return 0;
}
