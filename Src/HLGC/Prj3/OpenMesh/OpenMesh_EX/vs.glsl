#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
uniform mat4 pm;
uniform mat4 mm;
uniform float time;
out vec2 coord;
flat out vec4 color;
void main()
{
	gl_Position = pm * mm * position;
	coord = texcoord;
}