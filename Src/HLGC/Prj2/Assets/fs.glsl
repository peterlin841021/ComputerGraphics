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
	vec2 r = gl_FragCoord.xy/img_size.xy;
	
	fragmentcolor = texture(tex,vec2(r.x * 0.1*cos(time),r.y * 0.15*cos(time)));
	// if (temp.a < 0.1)
	// 	discard;
	// else		 
	//fragmentcolor = vec4(vec3(1,1,0), 1.0);
}