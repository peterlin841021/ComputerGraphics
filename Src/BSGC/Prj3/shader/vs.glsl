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
// out vec3 vs_worldpos;
// out vec3 vs_normal;
// out vec3 RefractVec;
// out vec3 ReflectVec;
out vec2 texturecoord;
out vec3 fcolor;
out vec3 texturecoord3d;
out vec3 vposition;
void main(void)
{
    vposition = position;
    vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(position,1.0); 
    //vec3 InVec = normalize(pos.xyz);
    //RefractVec = refract(InVec, normal, 0.66);
    //ReflectVec = reflect(InVec, normal);
    //vs_worldpos = pos.xyz;
    //vec3 p = normalize(pos.xyz);
    //vs_normal = vec3(p.x,p.y,p.z); 
    if(effect == 2)
        gl_Position = pos.xyww;
    else
        gl_Position = pos;
    texturecoord = uv;
    texturecoord3d = position;
    fcolor = color;
}
