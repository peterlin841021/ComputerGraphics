#version 430 core
layout(triangles,fractional_odd_spacing,cw) in;
in vec3 tcposition[];
out vec3 teposition[];
out vec3 tepatchdistance[];//Geometry

uniform bool usetessle;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

void main()
{
    if(usetessle)
    {

    }else
    {
        gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(tcposition[0],1.0);
    }
}
