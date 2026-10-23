#include "window2D.h"

namespace {
	std::string getFile(const std::string &path) {
		std::ifstream file(path);
			
		if(file) {
			std::stringstream buffer;
			
			buffer << file.rdbuf();
			
			return buffer.str();
		}
		
		throw errno;
	}
}

//TODO: Maybe add spritesheet padding support and change back to GL_LINEAR_MIPMAP_LINEAR and GL_LINEAR (Or make it possible to choose the filter)
Image::Image(const char* path) {
	stbi_set_flip_vertically_on_load(true);
	
	int numColCh;
	
	unsigned char* bytes = stbi_load(path, &width, &height, &numColCh, 4);
	
	glGenTextures(1, &texture);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	stbi_image_free(bytes);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

Image::Image(unsigned char* bytes, int width, int height) {
	this->width = width;
	this->height = height;
	
	glGenTextures(1, &texture);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

Image::Image(const Image &that) {
	width = that.width;
	height = that.height;
	
	glGenTextures(1, &texture);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	GLuint fbo;
	
	glGenFramebuffers(1, &fbo);
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, that.texture, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture, 0);
	
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glDeleteFramebuffers(1, &fbo);
}

Image::Image(Image &&that) {
	width = that.width;
	height = that.height;
	
	texture = that.texture;
	that.texture = 0;
}

Image &Image::operator=(Image that) {
	width = that.width;
	height = that.height;
	
	std::swap(texture, that.texture);
	
	return *this;
}

Image::~Image() {
	glDeleteTextures(1, &texture);
}

Character::Character(unsigned char* bytes, int width, int height, int bearingX, int bearingY, unsigned int advance) {
	glGenTextures(1, &texture);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	this->width = width;
	this->height = height;
	
	this->bearingX = bearingX;
	this->bearingY = bearingY;
	
	this->advance = advance;
}

Font::Font(const FT_Library &ft, const char* path) {
	FT_Face face;
	
	FT_New_Face(ft, path, 0, &face);
	
	FT_Set_Pixel_Sizes(face, 0, 64);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	for(unsigned char c = 32; c < 127; c ++) {
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		
		characters.insert(std::make_pair(c, Character(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top, static_cast<unsigned int>(face->glyph->advance.x))));
	}
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	
	FT_Done_Face(face);
}

Font::Font() {}

Attribute::Attribute(GLuint index, GLuint size, GLenum type, GLsizei stride, void* offset) {
	this->index = index;
	this->size = size;
	this->type = type;
	this->stride = stride;
	this->offset = offset;
}

Program::Program(const std::string &vertexPath, const std::string &fragmentPath, std::vector<Attribute> attributes, std::function<void(const Primitive &)> setUniforms) {
	std::string vertexCode = getFile(vertexPath);
	std::string fragmentCode = getFile(fragmentPath);
	
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);
	
	char vInfoLog[1024];
	glGetShaderInfoLog(vertexShader, 1024, nullptr, vInfoLog);
	
	std::cout << vertexPath << " " << vInfoLog << "\n";
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);
	
	char fInfoLog[1024];
	glGetShaderInfoLog(fragmentShader, 1024, nullptr, fInfoLog);
	
	std::cout << fragmentPath << " " << fInfoLog << "\n";
	
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	this->attributes = std::move(attributes);
	
	this->setUniforms = std::move(setUniforms);
}

Program::Program() {}

Primitive::Primitive(GLenum type, Program program, std::vector<GLfloat> vertices, std::vector<GLuint> indices, glm::mat4 matrix, GLuint texture) : program(std::move(program)) {
	this->type = type;
	
	this->vertices = std::move(vertices);
	this->indices = std::move(indices);
	
	this->matrix = std::move(matrix);
	
	this->texture = texture;
}

Window2D::Window2D(int width, int height, std::string_view title, bool resizable) {
	this->width = width;
	this->height = height;
	
	orthoMatrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
	
	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
	
	window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
	
	glfwMakeContextCurrent(window);
	
	glfwSetWindowUserPointer(window, this);
	
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		Window2D* window2D = static_cast<Window2D*>(glfwGetWindowUserPointer(window));
		
		window2D->width = width;
		window2D->height = height;
		
		window2D->orthoMatrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
	});
	
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	});
	
	gladLoadGL();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_MULTISAMPLE);
	
	glActiveTexture(GL_TEXTURE0);
	
	coloredProgram = Program("shaders/colored.vert", "shaders/colored.frag", {Attribute(0, 2, GL_FLOAT, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0)), Attribute(1, 4, GL_FLOAT, 6 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)))}, [this](const Primitive &primitive) {
		glUniformMatrix4fv(glGetUniformLocation(coloredProgram.ID, "uMatrix"), 1, GL_FALSE, glm::value_ptr(primitive.matrix));
	});
	texturedProgram = Program("shaders/textured.vert", "shaders/textured.frag", {Attribute(0, 2, GL_FLOAT, 4 * sizeof(GLfloat), reinterpret_cast<void*>(0)), Attribute(1, 2, GL_FLOAT, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)))}, [this](const Primitive &primitive) {
		glUniformMatrix4fv(glGetUniformLocation(texturedProgram.ID, "uMatrix"), 1, GL_FALSE, glm::value_ptr(primitive.matrix));
		
		glBindTexture(GL_TEXTURE_2D, primitive.texture);
	});
	textProgram = Program("shaders/text.vert", "shaders/text.frag", {Attribute(0, 2, GL_FLOAT, 4 * sizeof(GLfloat), reinterpret_cast<void*>(0)), Attribute(1, 2, GL_FLOAT, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)))}, [this](const Primitive &primitive) {
		glUniformMatrix4fv(glGetUniformLocation(textProgram.ID, "uMatrix"), 1, GL_FALSE, glm::value_ptr(primitive.matrix));
		
		glUniform4f(glGetUniformLocation(textProgram.ID, "uTextColor"), fillColor[0], fillColor[1], fillColor[2], globalAlpha);
		
		glBindTexture(GL_TEXTURE_2D, primitive.texture);
	});
	
	glUniform1i(glGetUniformLocation(texturedProgram.ID, "uImage"), 0);
	glUniform1i(glGetUniformLocation(textProgram.ID, "uImage"), 0);
	
	FT_Init_FreeType(&ft);
	
	font = Font(ft, "fonts/Arial.ttf");
}

void Window2D::setMouseMoveCallback(GLFWcursorposfun callback) {
	glfwSetCursorPosCallback(window, callback);
}

void Window2D::setMouseButtonCallback(GLFWmousebuttonfun callback) {
	glfwSetMouseButtonCallback(window, callback);
}

void Window2D::setKeyCallback(GLFWkeyfun callback) {
	glfwSetKeyCallback(window, callback);
}

bool Window2D::shouldClose() {
	return glfwWindowShouldClose(window);
}

void Window2D::fill(float r, float g, float b) {
	fillColor[0] = r;
	fillColor[1] = g;
	fillColor[2] = b;
}

glm::mat4 &Window2D::getCurrentMatrix() {
	return matrixStack[matrixStack.size() - 1];
}

void Window2D::setCurrentMatrix(glm::mat4 matrix) {
	matrixStack[matrixStack.size() - 1] = std::move(matrix);
}

void Window2D::save() {
	matrixStack.push_back(getCurrentMatrix());
}

void Window2D::translate(float x, float y) {
	setCurrentMatrix(glm::translate(getCurrentMatrix(), glm::vec3(x, y, 0)));
}

void Window2D::rotate(float angle) {
	setCurrentMatrix(glm::rotate(getCurrentMatrix(), angle, glm::vec3(0, 0, 1)));
}

void Window2D::scale(float x, float y) {
	setCurrentMatrix(glm::scale(getCurrentMatrix(), glm::vec3(x, y, 0)));
}

void Window2D::restore() {
	matrixStack.pop_back();
	
	if(matrixStack.empty()) {
		matrixStack.push_back(glm::identity<glm::mat4>());
	}
}

void Window2D::fillRect(float x, float y, float width, float height) {
	primitives.push_back(Primitive(GL_TRIANGLES, coloredProgram, {
		x, y, fillColor[0], fillColor[1], fillColor[2], globalAlpha,
		x + width, y, fillColor[0], fillColor[1], fillColor[2], globalAlpha,
		x, y + height, fillColor[0], fillColor[1], fillColor[2], globalAlpha,
		x + width, y + height, fillColor[0], fillColor[1], fillColor[2], globalAlpha
	}, {
		0, 1, 2,
		1, 2, 3
	}, orthoMatrix * getCurrentMatrix()));
}

void Window2D::drawImage(const Image &image, float x, float y) {
	primitives.push_back(Primitive(GL_TRIANGLES, texturedProgram, {
		x, y, 0.0f, 1.0f,
		x + image.width, y, 1.0f, 1.0f,
		x, y + image.height, 0.0f, 0.0f,
		x + image.width, y + image.height,
		1.0f, 0.0f
	}, {
		0, 1, 2,
		1, 2, 3
	}, orthoMatrix * getCurrentMatrix(), image.texture));
}

void Window2D::drawImage(const Image &image, float x, float y, float width, float height) {
	primitives.push_back(Primitive(GL_TRIANGLES, texturedProgram, {
		x, y, 0.0f, 1.0f,
		x + width, y, 1.0f, 1.0f,
		x, y + height, 0.0f, 0.0f,
		x + width, y + height, 1.0f, 0.0f
	}, {
		0, 1, 2,
		1, 2, 3
	}, orthoMatrix * getCurrentMatrix(), image.texture));
}

void Window2D::drawImage(const Image &image, float sX, float sY, float sWidth, float sHeight, float x, float y, float width, float height) {
	primitives.push_back(Primitive(GL_TRIANGLES, texturedProgram, {
		x, y, sX / image.width, 1.0f - sY / image.height,
		x + width, y, (sX + sWidth) / image.width, 1.0f - sY / image.height,
		x, y + height, sX / image.width, 1.0f - (sY + sHeight) / image.height,
		x + width, y + height, (sX + sWidth) / image.width, 1.0f - (sY + sHeight) / image.height
	}, {
		0, 1, 2,
		1, 2, 3
	}, orthoMatrix * getCurrentMatrix(), image.texture));
}

float Window2D::getTextWidth(std::string_view text) {
	float width = 0;
	
	float scale = textSize / 64.0;
	
	for(char c : text) {
		Character &character = font.characters.at(c);
		
		width += (character.advance >> 6) * scale;
	}
	
	return width;
}

void Window2D::text(std::string_view text, float x, float y) {
	if(textAlign == TextAlign::center) {
		x -= getTextWidth(text) / 2;
	}else if(textAlign == TextAlign::right) {
		x -= getTextWidth(text);
	}
	
	glDepthMask(GL_FALSE);
	
	float scale = textSize / 64.0;
	
	float penX = x;
	
	for(char c : text) {
		//Must use .at and not [] operator because c++ must create member if it does not exist and Character has no default constructor
		Character &character = font.characters.at(c);
		
		float xPos = penX + character.bearingX * scale;
		float yPos = y + (character.height - character.bearingY) * scale;
		
		float width = character.width * scale;
		float height = character.height * scale;
		
		primitives.push_back(Primitive(GL_TRIANGLES, textProgram, {
			xPos, yPos - height, 0.0f, 0.0f,
			xPos + width, yPos - height, 1.0f, 0.0f,
			xPos, yPos, 0.0f, 1.0f,
			xPos + width, yPos, 1.0f, 1.0f
		}, {
			0, 1, 2,
			1, 2, 3
		}, orthoMatrix * getCurrentMatrix(), character.texture));
		
		penX += (character.advance >> 6) * scale;
	}
	
	glDepthMask(GL_TRUE);
}

void Window2D::hideCursor() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Window2D::showCursor() {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window2D::render() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	for(Primitive &primitive : primitives) {
		glUseProgram(primitive.program.ID);
		
		GLuint VAO;
		
		glGenVertexArrays(1, &VAO);
		
		glBindVertexArray(VAO);
		
		GLuint VBO, EBO;
		
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, primitive.vertices.size() * sizeof(GLfloat), primitive.vertices.data(), GL_STATIC_DRAW);
		
		for(const Attribute &attribute : primitive.program.attributes) {
			glVertexAttribPointer(attribute.index, attribute.size, attribute.type, GL_FALSE, attribute.stride, attribute.offset);
			glEnableVertexAttribArray(attribute.index);
		}
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.indices.size() * sizeof(GLuint), primitive.indices.data(), GL_STATIC_DRAW);
		
		primitive.program.setUniforms(primitive);
		
		glDrawElements(primitive.type, GLsizei(primitive.indices.size()), GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	
	primitives.clear();
}

void Window2D::update() {
	glfwSwapBuffers(window);
	
	glfwPollEvents();
}

void Window2D::terminate() {
	FT_Done_FreeType(ft);
	
	glfwTerminate();
}
