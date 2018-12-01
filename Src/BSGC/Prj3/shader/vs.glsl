#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 normal;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform int colormode;
uniform int effect;
out vec3 half_vector;
out vec2 texturecoord;
out vec3 fcolor;
out vec3 texturecoord3d;
out vec2 tc;
void main(void)
{
    tc = position.xz;
    vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(position,1.0);
    if(effect == 2)
        gl_Position = pos.xyww;
    else if(effect == 7)
        gl_Position = vec4(position,1.0);
    else
        gl_Position = pos;
    texturecoord = uv;
    texturecoord3d = position;
    fcolor = color;
}
