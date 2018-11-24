#version 430 core
uniform sampler2D tex;
// uniform int shaderNumber;
// uniform float time;


out vec4 fColor;
// vec3 N; // eye space normal
// vec3 L; // eye space light vector
// vec3 H; // eye space halfway vector
in vec2 texcoord; //座標

/*-----------------*/
void main()
{
	// vec3 texColor = texture(tex,texcoord).rgb
	// fColor = vec4(texColor, 1.0);
	//fColor = vec4(vec3(1,1,1), 1.0);
}
