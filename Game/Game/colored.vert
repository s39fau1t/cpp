#version 330 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in vec4 a_color;

out vec4 v_color;

uniform vec2 u_size;

void main() {
	vec2 zeroToOne = a_position / u_size;
	vec2 zeroToTwo = zeroToOne * 2.0f;
	vec2 clipSpace = zeroToTwo - 1.0f;
	
	gl_Position = vec4(clipSpace.x, clipSpace.y * -1.0, 0.0f, 1.0);
	
	v_color = a_color;
}
