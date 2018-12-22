#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform int effect;
uniform int objType;
uniform bool useTessllation;
out vec2 texturecoord;
out vec3 texturecoord3d;
out vec2 tc;
out vec3 vs_normal;
out vec3 light_vector;
vec3 light_position = vec3(30.f,50.f,560.f);

void main(void)
{
    vec4 pos = ProjectionMatrix * ModelViewMatrix * vec4(position,1.0);
    vs_normal = normalize(ModelViewMatrix*vec4(normal,0)).xyz;
    vec4 mp = ModelViewMatrix * vec4(position,1.0);
    vec4 lp = ModelViewMatrix * vec4(light_position,1.0);
    light_vector = normalize(lp.xyz - mp.xyz);
    
    if(useTessllation)    
    {
        tc = position.xz;
        gl_Position = vec4(position,1.0);
    }
    else
    {
        if(objType == 1)//Skybox
        {
            gl_Position = pos.xyww;
            texturecoord3d = position;
        }
        else
        {
            gl_Position = pos;
            texturecoord = uv;
            texturecoord3d = position;
        }            
    }
}
