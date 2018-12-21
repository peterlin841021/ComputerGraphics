#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 normal;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform int colormode;
uniform int effect;
out vec2 texturecoord;
out vec3 fcolor;
out vec3 texturecoord3d;
out vec2 tc;
out vec3 vs_normal;
out vec3 light_vector;
vec3 light_position = vec3(30.f,50.f,560.f);
void main(void)
{
    tc = position.xz;
    vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(position,1.0);
    if(effect == 2)
        gl_Position = pos.xyww;
    else if(effect == 7 || effect == 9 || effect == 10)
        gl_Position = vec4(position,1.0);   
    else
        gl_Position = pos;
    texturecoord = uv;
    texturecoord3d = position;
    fcolor = color;
    //vs_normal = normal;
    vs_normal = normalize(ModelMatrix*vec4(vs_normal,0)).xyz;
    vec4 mp = ModelMatrix * vec4(position,1.0);    
    light_vector = normalize(light_position.xyz - mp.xyz );
}
