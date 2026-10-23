#version 300 es
precision highp float;

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D uImage;

void main() {
	FragColor = texture(uImage, texCoord);
}
