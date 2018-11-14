#version 430 core
uniform sampler2D Texture;
uniform int mode;
in vec2 vUV;
in vec4 colors;
out vec4 fColor;
void main()
{
	switch(mode)
	{
		case 0:
		fColor = colors;
		break;
		case 1:
		fColor =texture(Texture,vUV);
		break;
		case 2:
		fColor = colors;
		break;
	}	
}
