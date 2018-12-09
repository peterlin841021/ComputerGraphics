#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform float time;
uniform mat4 pm;
uniform mat4 mm;
flat out vec4 starColor;

void main(void)
{
	vec4 newVertex = position;

	newVertex.z += time;
	newVertex.z = fract(newVertex.z);

	float size = (20.0 * newVertex.z * newVertex.z);	
	newVertex.z = (999.9 * newVertex.z) - 1000.0;
	gl_Position = pm *mm* newVertex;
	gl_PointSize = size;
}