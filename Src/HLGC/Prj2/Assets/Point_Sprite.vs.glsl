#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform float time;
uniform mat4 pm;
uniform mat4 mm;
flat out vec4 starColor;
uniform int ps;
void main(void)
{
	vec4 newVertex = position;

	newVertex.z += time;
	newVertex.z = fract(newVertex.z);
	float density = 1000.0;
	float size = (20.0 * newVertex.z * newVertex.z);	
	newVertex.z = (density * newVertex.z) - density;
	gl_Position = pm * newVertex;
	gl_PointSize = size * ps;
}