#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

out vec4 color;

uniform mat4 uMatrix;

void main() {
	gl_Position = uMatrix * vec4(aPos, 0.0f, 1.0f);
	
	color = vec4(aColor.rgb / 255, aColor.a);
}
