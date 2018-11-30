#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 normal;
//layout(location = 4) in vec3 uv3d;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform int colormode;
uniform int effect;
uniform vec3 camerapos;
// out vec3 vs_worldpos;
// out vec3 vs_normal;
out vec3 RefractVec;
out vec3 ReflectVec;
out vec2 texturecoord;
out vec3 fcolor;
out vec3 texturecoord3d;
out vec2 tc;
void main(void)
{
    tc = position.xz;
    vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(position,1.0); 
    vec3 InVec = normalize(pos.xyz - camerapos);
    RefractVec = refract(InVec, normalize(vec3(0,1,0)), 0.66);
    ReflectVec = reflect(InVec, normalize(vec3(0,1,0)));
    //vs_worldpos = pos.xyz;
    //vec3 p = normalize(pos.xyz);
    //vs_normal = vec3(p.x,p.y,p.z); 
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
