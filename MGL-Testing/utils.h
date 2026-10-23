#include <string>
#include <array>
#include <unordered_map>

#include <GL/glcorearb.h>

GLuint createProgram(const std::string &vertexPath, const std::string &fragmentPath);
GLuint createVertexArray();
template <typename DataType, std::size_t size>
GLuint createBuffer(const std::array<DataType, size> &data, GLenum usage = GL_STATIC_DRAW) {
	GLuint buffer;

	glCreateBuffers(1, &buffer);

	glNamedBufferData(buffer, data.size() * sizeof(DataType), data.data(), usage);

	return buffer;
}
GLuint createTexture(GLenum target, const std::unordered_map<GLenum, GLint> &params, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels, bool mipmap);
void specifyAttrib(GLuint VAO, GLuint attribIndex, GLuint VBOIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset);
