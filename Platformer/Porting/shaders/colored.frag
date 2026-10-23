#version 300 es
precision highp float;

uniform vec4 uColor;

out vec4 FragColor;

void main() {
	FragColor = vec4(uColor.rgb / 255.0, uColor.a);
}
