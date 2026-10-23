#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D uImage;

void main() {
	FragColor = texture(uImage, texCoord);
}
