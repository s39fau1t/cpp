#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 uMatrix;

void main() {
	gl_Position = uMatrix * vec4(aPos, 0.0f, 1.0f);
}
