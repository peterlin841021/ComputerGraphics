#version 430

in vec3 VertexColor;
out vec4 outColor;

void main()
{
    outColor = vec4(VertexColor,1.0);
    //outColor = vec4(0,0,1,1.0);
}