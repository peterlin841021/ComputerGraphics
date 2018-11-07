#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform vec3 normal;
out vec3 vs_worldpos;
out vec3 vs_normal;
out vec2 vUV;
out vec3 RefractVec;
out vec3 ReflectVec;
out vec3 box_pos;
void main(void)
{    
    vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(position,1.0); 
    vec3 InVec = normalize(pos.xyz); 
    RefractVec = refract(InVec, normal, 0.66);
    ReflectVec = reflect(InVec, normal);      
    gl_Position = pos;    
    vs_worldpos = pos.xyz;
    box_pos = position;
    vec3 p = normalize(pos.xyz);
    vs_normal = vec3(p.x,p.y,p.z);
    vUV = uv;    
    
}
