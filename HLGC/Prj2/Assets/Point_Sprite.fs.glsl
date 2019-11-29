#version 410 core

out vec4 color;

uniform sampler2D tex_star;
flat in vec4 starColor;
uniform mat4 mm;
uniform int darken;
void Darken()
{	
	if(darken == 1)
	{
		color = vec4(color.r-0.5*color.r,color.g-0.5*color.g,color.b-0.5*color.b,color.a); 
	} 
}
void main(void)
{	
	vec2 coord = gl_PointCoord;	
	//coord = (mm * vec4(coord,0,1)).xy;
	color = texture(tex_star,coord);
	if(darken == 1)
		Darken();
	if (color.a < 0.1)
		discard;
}