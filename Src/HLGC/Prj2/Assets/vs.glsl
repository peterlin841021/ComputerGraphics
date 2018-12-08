#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
uniform mat4 pm;
uniform mat4 mm;
uniform int type;
uniform float time;
out vec2 coord;
flat out vec4 color;
void main()
{
	if(type == 0)
	{
		gl_Position = pm * mm * vec4(position, 1.0);
		coord = texcoord;
	}		
	else if(type == 1)
	{
		// vec4 newvtx = vec4(position,1);
		// newvtx.z += time;
		// newvtx.z = fract(newvtx.z);

		// float pointsize = (20.0 * newvtx.z * newvtx.z);
		// color = smoothstep(1.0, 7.0, pointsize) * vec4(1,1,0,1);
		// newvtx.z = (999.9 * newvtx.z ) - 1000.0;
		// gl_Position = pm * newvtx;
		gl_PointSize = 0.5;
		gl_Position = pm  * mm * vec4(position, 1.0);
	}
}