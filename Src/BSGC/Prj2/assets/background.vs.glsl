#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
out vec2 vUV;

void main(void)
{
    gl_Position = vec4(vertex, 1.0);
    vUV = uv;
}
