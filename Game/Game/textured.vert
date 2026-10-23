#version 330 core

layout (location = 0) in vec2 a_position;
layout (location = 1) in float a_alpha;
layout (location = 2) in vec2 a_texCoord;

out float v_alpha;
out vec2 v_texCoord;

uniform vec2 u_size;

void main() {
	vec2 zeroToOne = a_position / u_size;
	vec2 zeroToTwo = zeroToOne * 2.0f;
	vec2 clipSpace = zeroToTwo - 1.0f;
	
	gl_Position = vec4(clipSpace.x, clipSpace.y * -1.0, 0.0f, 1.0);
	
	v_alpha = a_alpha;
	v_texCoord = a_texCoord;
}
