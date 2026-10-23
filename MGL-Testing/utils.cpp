#include <fstream>

#include "GL/glcorearb.h"
#include "utils.h"

namespace {
	std::string getFile(const std::string &path) {
		std::ifstream file(path);

		return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	}

	GLuint createShader(const std::string &path, GLenum type) {
		std::string code = getFile(path);
		const char* source = code.c_str();

		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);

		return shader;
	}
}

GLuint createProgram(const std::string &vertexPath, const std::string &fragmentPath) {
	GLuint program = glCreateProgram();

	GLuint vertexShader = createShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(fragmentPath, GL_FRAGMENT_SHADER);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	//FIXME: Function to delete the shaders is not implemented yet (asserts 0)
	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);

	return program;
}

GLuint createVertexArray() {
	GLuint VAO;

	glCreateVertexArrays(1, &VAO);

	return VAO;
}

void specifyAttrib(GLuint VAO, GLuint attribIndex, GLuint VBOIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset) {
	glEnableVertexArrayAttrib(VAO, attribIndex);
	glVertexArrayAttribBinding(VAO, attribIndex, VBOIndex);
	glVertexArrayAttribFormat(VAO, attribIndex, size, type, normalized, relativeOffset);
}

GLuint createTexture(GLenum target, const std::unordered_map<GLenum, GLint> &params, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels, bool mipmap) {
	GLuint texture;

	glCreateTextures(target, 1, &texture);

	for(auto [pname, param] : params) {
		glTextureParameteri(texture, pname, param);
	}

	glTextureStorage2D(texture, levels, internalFormat, width, height);

	glTextureSubImage2D(texture, 0, 0, 0, width, height, format, type, pixels);

	if(mipmap) {
		glGenerateTextureMipmap(texture);
	}

	return texture;
}
