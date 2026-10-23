//https://www.shadertoy.com/view/ls3SDx

uniform sampler2D texture;

uniform vec2 resolution;
uniform float time;

float perspective = 0.3;

const int samples = 25;
const float minBlur = .1;
const float maxBlur = .3;
const float speed = 3.;

void main() {
	vec2 p = gl_FragCoord.xy / resolution.xy;

	vec4 result = vec4(0);

	float timeQ = mix(minBlur, maxBlur, (sin(time*speed)+1.)/2.);

	for (int i=0; i<=samples; i++) {        
		float q = float(i)/float(samples);
		result += texture2D(texture, p + (vec2(0.5)-p)*q*timeQ)/float(samples);
	}


	gl_FragColor = result;
}
