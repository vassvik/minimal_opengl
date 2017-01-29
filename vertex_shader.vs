#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform float time;
uniform vec2 resolution;

#define PI 3.1415926

void main(){
	// time-dependent scaling, pulse once per second
	float frequency = 1.0;
	float scale = (0.6 + 0.4*cos(frequency*2.0*PI*time));

	vec3 pos = vertexPosition * scale;
	
	// fix aspect ratio
	// by stretching x-axis
	pos.x /= (resolution.x/resolution.y);

    gl_Position = vec4(pos, 1.0);
}

