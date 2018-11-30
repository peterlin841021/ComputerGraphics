#version 430 core
#define ID gl_InvocationID

layout(vertices = 3) out;
in vec3 vposition[];
out vec3 tcposition[];

uniform bool usetessle;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform float zoom;
uniform float tesslevelInner;
uniform float tesslevelOuter;
void main()
{
    tcposition[ID] = vposition[ID];
    vec4 p1 = ProjectionMatrix * ModelViewMatrix * vec4(vposition[0],1.0);
    vec4 p2 = ProjectionMatrix * ModelViewMatrix * vec4(vposition[1],1.0);
    vec4 p3 = ProjectionMatrix * ModelViewMatrix * vec4(vposition[2],1.0);
    if(usetessle)
    {

    }else
    {
        
    }
}