#version 300 es

/*layout (location = 0) */in vec2 aPos;
/*layout (location = 1) */in vec2 aTex;

out vec2 texCoord;

uniform mat4 uMatrix;

void main() {
	gl_Position = uMatrix * vec4(aPos, 0.0f, 1.0f);
	
	texCoord = aTex;
}
