#version 460 core

layout (location = 0) in vec2 texCoord;

layout (location = 0) out vec4 FragColor;

uniform sampler2D image;

void main() {
	FragColor = texture(image, texCoord);
}
