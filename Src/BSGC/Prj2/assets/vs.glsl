#version 430 core
layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;

uniform mat4 mv;
uniform mat4 pm;
// vec3 N; // eye space normal
// vec3 L; // eye space light vector
// vec3 H; // eye space halfway vector
out vec2 texcoord;

void main()
{
	gl_Position = pm * mv * vec4(iv3vertex, 1.0);    
    texcoord = iv2tex_coord;  
}