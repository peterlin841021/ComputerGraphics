#version 410 core

out vec4 color;

uniform sampler2D tex_star;
flat in vec4 starColor;
uniform mat4 mm;
void main(void)
{	
	vec2 coord = gl_PointCoord;	
	//coord = (mm * vec4(coord,0,1)).xy;
	color = texture(tex_star,coord);
	if (color.a < 0.1)
		discard;
}