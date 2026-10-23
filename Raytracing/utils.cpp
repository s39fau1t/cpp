#include "utils.h"
#include "bgfx/bgfx.h"

#include <fstream>

#include <stb/stb_image.h>

namespace {
	const bgfx::Memory* loadFile(const std::string &path) {
		std::ifstream file(path);

		file.seekg(0, std::ios::end);

		int size = file.tellg();

		const bgfx::Memory* data = bgfx::alloc(size * sizeof(char));

		file.seekg(0, std::ios::beg);

		file.read(reinterpret_cast<char*>(data->data), size);

		return data;
	}
}

bgfx::ProgramHandle createProgram(const std::string &vertexPath, const std::string &fragmentPath) {
	bgfx::ShaderHandle vertexShader = bgfx::createShader(loadFile(vertexPath));
	bgfx::ShaderHandle fragmentShader = bgfx::createShader(loadFile(fragmentPath));

	bgfx::ProgramHandle program = bgfx::createProgram(vertexShader, fragmentShader);

	bgfx::destroy(vertexShader);
	bgfx::destroy(fragmentShader);

	return program;
}

bgfx::ProgramHandle createProgram(const std::string &computePath) {
	bgfx::ShaderHandle computeShader = bgfx::createShader(loadFile(computePath));

	bgfx::ProgramHandle program = bgfx::createProgram(computeShader);

	bgfx::destroy(computeShader);

	return program;
}

bgfx::TextureHandle createTexture2D(const std::string &path, bgfx::TextureFormat::Enum format) {
	int width;
	int height;
	int colorChannels;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &colorChannels, 0);

	int size = width * height * colorChannels;

	const bgfx::Memory* memory = bgfx::makeRef(data, size, [](void* data, void* userData) {
		stbi_image_free(data);
	});

	return bgfx::createTexture2D(width, height, false, 1, format, 0 | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP, memory);
}
