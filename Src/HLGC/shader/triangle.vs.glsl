#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 0) uniform mat4 mm;
layout (location = 1) uniform mat4 pm;
out vec3 fcolor;
void main()
{
    //gl_Position = vec4(position,1.0f);
    gl_Position = pm*mm*vec4(position,1.0);
    fcolor = color;
}