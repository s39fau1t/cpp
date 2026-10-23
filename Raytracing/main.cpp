#include "bgfx/defines.h"
#include <algorithm>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdint>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#include <bgfx/bgfx.h>

#include <glm/glm.hpp>

#include "utils.h"

constexpr int width = 800;
constexpr int height = 800;

float vertices[] = {
	//x    y       s  t
	 -1,   1,      0, 1,
	  1,   1,      1, 1,
	 -1,  -1,      0, 0,
	  1,  -1,      1, 0
};

/*
0      1


2      3
*/

uint16_t indices[] = {
	0, 1, 2,
	1, 2, 3
};

constexpr int numSpheres = 3;

float spheres[numSpheres * 7 + 1] = {
	numSpheres,

	0, -1, 3,
	1,
	255, 0, 0,

	2, 0, 4,
	1,
	0, 0, 255,

	-2, 0, 4,
	1,
	0, 255, 0
};

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
	layout.begin().add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float).add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float).end();

	bgfx::VertexBufferHandle buffer = bgfx::createVertexBuffer(bgfx::makeRef(vertices, sizeof(vertices)), layout);
	bgfx::IndexBufferHandle indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

	bgfx::VertexLayout spheresLayout;
	spheresLayout.begin().add(bgfx::Attrib::Count, 3, bgfx::AttribType::Float).add(bgfx::Attrib::Count, 1, bgfx::AttribType::Float).add(bgfx::Attrib::Count, 3, bgfx::AttribType::Float).end();

	bgfx::VertexBufferHandle spheresBuffer = bgfx::createVertexBuffer(bgfx::makeRef(spheres, sizeof(spheres)), spheresLayout, BGFX_BUFFER_COMPUTE_READ);
	bgfx::TextureHandle computeOut = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_COMPUTE_WRITE);

	bgfx::ProgramHandle computeProgram = createProgram("shaders/cs_ray.bin");

	bgfx::ProgramHandle program = createProgram("shaders/vs_tex.bin", "shaders/fs_tex.bin");

	bgfx::UniformHandle s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

	//TODO: This should go in loop
	bgfx::setBuffer(0, spheresBuffer, bgfx::Access::Read);
	bgfx::setImage(1, computeOut, 0, bgfx::Access::Write, bgfx::TextureFormat::RGBA8);

	bgfx::dispatch(0, computeProgram);

	while(!glfwWindowShouldClose(window)) {
		bgfx::setVertexBuffer(0, buffer);
		bgfx::setIndexBuffer(indexBuffer);
		bgfx::setTexture(0, s_texColor, computeOut);
		bgfx::setState(BGFX_STATE_DEFAULT ^ BGFX_STATE_CULL_CW);

		bgfx::submit(0, program);

		bgfx::frame();
		glfwPollEvents();
	}

	bgfx::shutdown();
	glfwTerminate();
}
