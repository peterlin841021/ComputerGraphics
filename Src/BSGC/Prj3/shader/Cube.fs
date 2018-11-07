#version 430 core
//in vec2 uv;
//uniform samplerCube box;
uniform sampler2D box;
in vec2 textureCoords;
out vec4 color;
void main()
{	
	//color = texture2D(box,uv);
	color = texture2D(box,textureCoords);
	//vec3 t  = abs(textureCoords);
}
