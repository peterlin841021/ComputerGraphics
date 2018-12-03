#version 430 core
layout(quads, equal_spacing,cw) in;
in vec2 tc_out[];
out vec2 tc;
out vec3 vpos;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform sampler2D heightmap;
uniform float time;
void main()
{  
    vec2 tc1 = mix(tc_out[0], tc_out[3], gl_TessCoord.x);
	vec2 tc2 = mix(tc_out[1], tc_out[2], gl_TessCoord.x);
	vec2 tc3 = mix(tc2, tc1, gl_TessCoord.y);
	float height = 20 * sin(gl_TessCoord.x + time * 20);
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[1].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);    	
	vec4 p = mix(p2, p1, gl_TessCoord.y);
	tc = tc3 /(sin(time)* 100);
	//tc = tc3 / 1000;
    p.y += texture(heightmap,tc).r * 40;
	tc = tc3 / 1000;
	gl_Position = ProjectionMatrix * ModelViewMatrix * p;
	vpos = vec3(ModelViewMatrix * p);
}
