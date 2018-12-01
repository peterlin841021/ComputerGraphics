#version 430 core

layout(vertices = 4) out;
in vec2 tc[];
out vec2 tc_out[];
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

void main()
{
    int part = 64;
    if(gl_InvocationID == 0)
    {        
        gl_TessLevelInner[0] = part;
        gl_TessLevelInner[1] = part;

		gl_TessLevelOuter[0] = part;
		gl_TessLevelOuter[1] = part;
		gl_TessLevelOuter[2] = part;
        gl_TessLevelOuter[3] = part;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tc_out[gl_InvocationID] = tc[gl_InvocationID];
}