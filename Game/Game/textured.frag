#version 330 core

in float v_alpha;
in vec2 v_texCoord;

out vec4 FragColor;

uniform sampler2D u_image;

void main() {
	FragColor = vec4(texture(u_image, v_texCoord).rgb, v_alpha);
}
