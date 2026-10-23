#version 300 es
precision highp float;

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D uImage;
uniform vec4 uTextColor;

void main() {
	FragColor = vec4(uTextColor.rgb / 255.0, uTextColor.a * texture(uImage, texCoord).r);
}
