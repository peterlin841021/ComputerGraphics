#version 430 core
layout(quads, equal_spacing,cw) in;
in vec2 tc_out[];
in vec2 tc_uv[];
out vec2 tc;
out vec3 vpos;
out vec3 nor;
out vec2 uv;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform sampler2D heightmap;
uniform float time;
void main()
{  
    vec2 tc1 = mix(tc_out[0], tc_out[3], gl_TessCoord.x);
	vec2 tc2 = mix(tc_out[1], tc_out[2], gl_TessCoord.x);
	vec2 tc3 = mix(tc2, tc1, gl_TessCoord.y);
	
	vec4 p1 = mix(gl_in[0].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[1].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);    	
	vec4 p = mix(p2, p1, gl_TessCoord.y);	
	uv = tc3/(30*30);
	
    p.y += texture(heightmap,uv).r * 100 * (sin(time+p.x));
	//p.x += sin(asin(p.y));
	gl_Position = ProjectionMatrix * ModelViewMatrix * p;	
	vpos = vec3(ModelViewMatrix * p);
	//	
	float height_diff1 = texture(heightmap, tc3 - vec2(0.01, 0)).r - texture(heightmap, tc3 + vec2(0.01, 0)).r;
	float height_diff2 = texture(heightmap, tc3 - vec2(0, 0.01)).r - texture(heightmap, tc3 + vec2(0, 0.01)).r;

	nor = normalize(vec3(height_diff1, 2.0, height_diff2));
}
