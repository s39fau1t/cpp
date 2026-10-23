#include "openGl2D.h"

string getFile(string filename) {
	ifstream file(filename, ios::binary);
	
	if(file) {
		string contents;
		
		file.seekg(0, ios::end);
		
		contents.resize(file.tellg());
		
		file.seekg(0, ios::beg);
		
		file.read(&contents[0], contents.size());
		
		file.close();
		
		return contents;
	}
	
	throw errno;
}

Image::Image(const char* filename) {
	int width, height, numColCh;
	
	unsigned char* texture = stbi_load(filename, &width, &height, &numColCh, 0);
	
	this->texture = texture;
	
	this->height = height;
	this->width = width;
}

Primitive::Primitive(GLenum type, GLfloat verticies[], int verticiesLength, int dataPerVertex) {
	this->type = type;
	
	for(int i = 0; i < verticiesLength; i ++) {
		this->verticies.push_back(verticies[i]);
	}
	
	this->dataPerVertex = dataPerVertex;
	
	textured = false;
}

Primitive::Primitive(GLenum type, GLfloat verticies[], int verticiesLength, int dataPerVertex, unsigned char* texture, GLsizei textureWidth, GLsizei textureHeight) {
	this->type = type;
	
	for(int i = 0; i < verticiesLength; i ++) {
		this->verticies.push_back(verticies[i]);
	}
	
	this->dataPerVertex = dataPerVertex;
	
	this->texture = texture;
	
	textured = true;
	
	this->textureWidth = textureWidth;
	this->textureHeight = textureHeight;
}

GLuint createProgram(const char** vertexShaderSource, const char** fragmentShaderSource) {
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	
	GLuint program = glCreateProgram();
	
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	
	glLinkProgram(program);
	
	GLchar verexLog[1024];
	GLchar fragmentLog[1024];
	
	glGetShaderInfoLog(vertexShader, sizeof(verexLog), NULL, verexLog);
	glGetShaderInfoLog(fragmentShader, sizeof(fragmentLog), NULL, fragmentLog);
	
	cout << "Vertex Log:\n" << verexLog << "\n";
	cout << "Fragment Log:\n" << fragmentLog << "\n";
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	return program;
}

OpenGl2D::OpenGl2D(const char* name, bool maximized, int w, int h) {
	error = false;
	
	translateX = 0;
	translateY = 0;
	
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, maximized ? GL_TRUE : GL_FALSE);
	
	window = glfwCreateWindow(w, h, name, NULL, NULL);

	if(window == NULL) {
		cout << "Failed to create a GLFW window ;-;\n";
		error = true;
		
		return;
	}

	glfwMakeContextCurrent(window);
	
	glfwSetWindowUserPointer(window, this);
	
	auto sizeCallback = [](GLFWwindow* window, int width, int height) {
		OpenGl2D* ctx = (OpenGl2D*)glfwGetWindowUserPointer(window);
		
		ctx->width = width;
		ctx->height = height;
	};
	
	auto framebufferCallback = [](GLFWwindow* window, int width, int height) {
		OpenGl2D* ctx = (OpenGl2D*)glfwGetWindowUserPointer(window);
		
		ctx->pixelWidth = width;
		ctx->pixelHeight= height;
	};
	
	glfwSetWindowSizeCallback(window, sizeCallback);
	glfwSetFramebufferSizeCallback(window, framebufferCallback);
	
	glfwGetWindowSize(window, &width, &height);
	glfwGetFramebufferSize(window, &pixelWidth, &pixelHeight);

	gladLoadGL();
	
	string coloredVertexFile = getFile("colored.vert");
	string coloredFragmentFile = getFile("colored.frag");
	
	const char* coloredVertexSource = coloredVertexFile.c_str();

	const char* coloredFragmentSource = coloredFragmentFile.c_str();
	
	coloredProgram = createProgram(&coloredVertexSource, &coloredFragmentSource);
	
	string texturedVertexFile = getFile("textured.vert");
	string texturedFragmentFile = getFile("textured.frag");
	
	const char* texturedVertexSource = texturedVertexFile.c_str();
	const char* texturedFragmentSource = texturedFragmentFile.c_str();
	
	texturedProgram = createProgram(&texturedVertexSource, &texturedFragmentSource);
	
	glGenVertexArrays(1, &VAO);
	
	glGenBuffers(1, &VBO);
	
	glGenTextures(1, &texture);
	
	glActiveTexture(GL_TEXTURE0);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	GLfloat color[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	
	coloredSizeUniformLoc = glGetUniformLocation(coloredProgram, "u_size");
	texturedSizeUniformLoc = glGetUniformLocation(texturedProgram, "u_size");
	imageUniformLoc = glGetUniformLocation(texturedProgram, "u_image");
	
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	stbi_set_flip_vertically_on_load(true);
}

void OpenGl2D::setKeyCallback(GLFWkeyfun keyCallback) {
	glfwSetKeyCallback(window, keyCallback);
}

void OpenGl2D::setMouseMoveCallback(GLFWcursorposfun mouseMoveCallback) {
	glfwSetCursorPosCallback(window, mouseMoveCallback);
}

void OpenGl2D::setMouseButtonCallback(GLFWmousebuttonfun mouseButtonCallback) {
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

bool OpenGl2D::isRunning() {
	if(!glfwWindowShouldClose(window) && !error) {
		return true;
	}else {
		return false;
	}
}

double OpenGl2D::getTime() {
	return glfwGetTime();
}

void OpenGl2D::setTime(double time) {
	glfwSetTime(time);
}

void OpenGl2D::background(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a);
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	primitives.clear();
}

void OpenGl2D::setFillColor(GLfloat r, GLfloat g, GLfloat b) {
	fillColor[0] = r;
	fillColor[1] = g;
	fillColor[2] = b;
}

void OpenGl2D::translate(GLfloat x, GLfloat y) {
	translateX += x;
	translateY += y;
}

void OpenGl2D::resetTranslation() {
	translateX = 0;
	translateY = 0;
}

void OpenGl2D::rect(GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
	GLfloat verticies[] = {
		x + translateX, y + translateY, fillColor[0] / 255.0f, fillColor[1] / 255.0f, fillColor[2] / 255.0f, globalAlpha,
		x + w + translateX, y + translateY, fillColor[0] / 255.0f, fillColor[1] / 255.0f, fillColor[2] / 255.0f, globalAlpha,
		x + translateX, y + h + translateY, fillColor[0] / 255.0f, fillColor[1] / 255.0f, fillColor[2] / 255.0f, globalAlpha,
		x + w + translateX, y + translateY, fillColor[0] / 255.0f, fillColor[1] / 255.0f, fillColor[2] / 255.0f, globalAlpha,
		x + translateX, y + h + translateY, fillColor[0] / 255.0f, fillColor[1] / 255.0f, fillColor[2] / 255.0f, globalAlpha,
		x + w + translateX, y + h + translateY, fillColor[0] / 255.0f, fillColor[1] / 255.0f, fillColor[2] / 255.0f, globalAlpha
	};
	
	primitives.push_back(Primitive(GL_TRIANGLES, verticies, sizeof(verticies) / sizeof(verticies[0]), 6));
}

void OpenGl2D::circle(GLfloat x, GLfloat y, GLfloat rad) {
	GLfloat color[] = {fillColor[0] / 255.0f, fillColor[1] / 255.0f, fillColor[2] / 255.0f, globalAlpha};
	
	vector<GLfloat> verticiesVector;
	
	verticiesVector.push_back(x + translateX);
	verticiesVector.push_back(y + translateY);
	
	for(GLfloat num : color) {
		verticiesVector.push_back(num);
	}
	
	for(int i = 0; i <= 20; i ++) {
		verticiesVector.push_back(x + (rad * cos(i * (M_PI * 2) / 20)) + translateX);
		verticiesVector.push_back(y + (rad * sin(i * (M_PI * 2) / 20)) + translateY);
		
		for(GLfloat num : color) {
			verticiesVector.push_back(num);
		}
	}
	
	GLfloat verticies[verticiesVector.size()];
	
	copy(verticiesVector.begin(), verticiesVector.end(), verticies);
	
	primitives.push_back(Primitive(GL_TRIANGLE_FAN, verticies, int(sizeof(verticies) / sizeof(verticies[0])), 6));
}

void OpenGl2D::drawImage(const Image &image, GLfloat x, GLfloat y) {
	GLfloat w = image.width, h = image.height;
	
	GLfloat verticies[] = {
		x + translateX, y + translateY,            globalAlpha,    0.0f, 1.0f,
		x + w + translateX, y + translateY,        globalAlpha,    1.0f, 1.0f,
		x + translateX, y + h + translateY,        globalAlpha,    0.0f, 0.0f,
		x + w + translateX, y + translateY,        globalAlpha,    1.0f, 1.0f,
		x + translateX, y + h + translateY,        globalAlpha,    0.0f, 0.0f,
		x + w + translateX, y + h + translateY,    globalAlpha,    1.0f, 0.0f
	};
	
	primitives.push_back(Primitive(GL_TRIANGLES, verticies, sizeof(verticies) / sizeof(verticies[0]), 5, image.texture, image.width, image.height));
}

void OpenGl2D::drawImage(const Image &image, GLfloat x, GLfloat y, GLfloat w, GLfloat h) {
	GLfloat verticies[] = {
		x + translateX, y + translateY,            globalAlpha,    0.0f, 1.0f,
		x + w + translateX, y + translateY,        globalAlpha,    1.0f, 1.0f,
		x + translateX, y + h + translateY,        globalAlpha,    0.0f, 0.0f,
		x + w + translateX, y + translateY,        globalAlpha,    1.0f, 1.0f,
		x + translateX, y + h + translateY,        globalAlpha,    0.0f, 0.0f,
		x + w + translateX, y + h + translateY,    globalAlpha,    1.0f, 0.0f
	};
	
	primitives.push_back(Primitive(GL_TRIANGLES, verticies, sizeof(verticies) / sizeof(verticies[0]), 5, image.texture, image.width, image.height));
}

void OpenGl2D::text(string text, GLfloat x, GLfloat y) {
	
}

void OpenGl2D::render() {
	glfwMakeContextCurrent(window);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glUseProgram(coloredProgram);
	
	glUniform2f(coloredSizeUniformLoc, width, height);
	
	glUseProgram(texturedProgram);
	
	glUniform2f(texturedSizeUniformLoc, width, height);
	
	glUniform1i(imageUniformLoc, 0);
	
	for(Primitive &primitive : primitives) {
		GLfloat verticiesArray[primitive.verticies.size()];
		
		copy(primitive.verticies.begin(), primitive.verticies.end(), verticiesArray);
		
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticiesArray), verticiesArray, GL_STATIC_DRAW);
		
		if(!primitive.textured) {
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, primitive.dataPerVertex * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, primitive.dataPerVertex * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);
			
			glUseProgram(coloredProgram);
		}else {
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, primitive.dataPerVertex * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			
			glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, primitive.dataPerVertex * sizeof(float), (void*)(2 * sizeof(float)));
			glEnableVertexAttribArray(1);
			
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, primitive.dataPerVertex * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, primitive.textureWidth, primitive.textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, primitive.texture);
			glGenerateMipmap(GL_TEXTURE_2D);
			
			glUseProgram(texturedProgram);
		}
		
		glDrawArrays(primitive.type, 0, int(primitive.verticies.size() / primitive.dataPerVertex));
	}
	
	glfwSwapBuffers(window);
	
	glfwPollEvents();
}

void OpenGl2D::terminate() {
	glDeleteProgram(coloredProgram);
	glDeleteProgram(texturedProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
}
