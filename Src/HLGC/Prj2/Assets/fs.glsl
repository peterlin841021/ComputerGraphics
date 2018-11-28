#version 410

in vec2 coord;
out vec4 fragmentcolor; 
uniform sampler2D tex;
uniform int effect;
uniform float time;
vec2 img_size = vec2(800., 340.);
const vec2 iResolution = vec2(800., 340.);
void main(void)
{			
	vec4 temp = texture(tex,coord);
	if (temp.a < 0.1)
		discard;
	else		 
		fragmentcolor = temp;
}