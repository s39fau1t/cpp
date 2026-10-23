#include "bgfx/defines.h"
#include <_types/_uint16_t.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <bgfx/bgfx.h>
#define BX_CONFIG_DEBUG 1
#include <bx/math.h>

constexpr int width = 800;
constexpr int height = 800;

float vertices[] = {
	//x    y        r  g  b
	-0.5, -0.5,     1, 0, 0,
	 0.5, -0.5,     0, 1, 0,
	-0.5,  0.5,     0, 0, 1,
	 0.5,  0.5,     0, 1, 1,
};


/*
2      3


0      1
*/

uint16_t indices[] = {
	0, 1, 2,
	2, 1, 3
};

const bgfx::Memory* loadFile(const std::string &path) {
	std::ifstream file(path);

	file.seekg(0, std::ios::end);

	int size = file.tellg();

	const bgfx::Memory* data = bgfx::alloc(size * sizeof(char));

	file.seekg(0, std::ios::beg);

	file.read(reinterpret_cast<char*>(data->data), size);

	return data;
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window = glfwCreateWindow(width, height, "bgfx", nullptr, nullptr);

	bgfx::Init init;
	init.type = bgfx::RendererType::Count;
	init.resolution.width = width;
	init.resolution.height = height;
	init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X16;
	init.platformData.nwh = glfwGetCocoaWindow(window);

	bgfx::init(init);

	bgfx::setViewRect(0, 0, 0, width, height);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);

	bgfx::VertexLayout layout;
	layout.begin().add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float).add(bgfx::Attrib::Color0, 3, bgfx::AttribType::Float).end();

	bgfx::VertexBufferHandle buffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), layout);
	bgfx::IndexBufferHandle indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

	const bgfx::Memory* vertexSource = loadFile("shaders/vs_tri.bin");
	const bgfx::Memory* fragmentSource = loadFile("shaders/fs_tri.bin");

	bgfx::ShaderHandle vertexShader = bgfx::createShader(vertexSource);
	bgfx::ShaderHandle fragmentShader = bgfx::createShader(fragmentSource);

	bgfx::ProgramHandle program = bgfx::createProgram(vertexShader, fragmentShader);

	bgfx::destroy(vertexShader);
	bgfx::destroy(fragmentShader);

	while(!glfwWindowShouldClose(window)) {
		float mtx[16];

		bx::mtxRotateZ(mtx, -glfwGetTime());

		bgfx::setTransform(mtx);

		bgfx::setVertexBuffer(0, buffer);
		bgfx::setIndexBuffer(indexBuffer);
		bgfx::setState(BGFX_STATE_DEFAULT ^ BGFX_STATE_CULL_CW);

		bgfx::submit(0, program);

		bgfx::frame();
		glfwPollEvents();
	}

	bgfx::shutdown();
	glfwTerminate();
}
