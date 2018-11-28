#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
uniform mat4 pm;
uniform mat4 mm;
out vec2 coord;
void main()
{
	//gl_Position = pm * mm * vec4(position, 1.0);
	gl_Position = vec4(position, 1.0);
	coord = texcoord;
}