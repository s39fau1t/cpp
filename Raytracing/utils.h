#include <string>

#include <bgfx/bgfx.h>

bgfx::ProgramHandle createProgram(const std::string &vertexPath, const std::string &fragmentPath);
bgfx::ProgramHandle createProgram(const std::string &computePath);
const bgfx::Memory* loadTexture(const std::string &path);
bgfx::TextureHandle createTexture2D(const std::string &path, bgfx::TextureFormat::Enum format);
