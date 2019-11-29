#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform int mode;
out vec2 vUV;
out vec4 colors;
void main(void)
{
    switch(mode)
    {
        case 0:
            gl_Position = vec4(vertex, 1.0);
            colors = vec4(color,1.0);
            vUV = uv;
        break;
        case 1:
            gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(vertex, 1.0);
            vUV = uv;
        break;
        case 2:
            gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(vertex, 1.0);
            colors = vec4(color,1.0);
        break;
    }    
}
