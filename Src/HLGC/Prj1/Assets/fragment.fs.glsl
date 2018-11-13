#version 410
layout(location = 0) out vec4 fragColor;
uniform int shaderNumber;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord;
} vertexData;

uniform sampler2D tex;
vec2 img_size = vec2(800, 800); 

void Shader_Normal(){		
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;
	fragColor = vec4(texColor, 1.0);	
}

void Shader_Gray(){	
	
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	float grayColor = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
	fragColor = vec4(grayColor,grayColor,grayColor, 1.0);
}

void shader_Quantization(int a){	
	vec3 texColor;
	if(a == 0)	
		texColor = texture(tex,vertexData.texcoord).rgb;	
	else	
		texColor =  vec3(fragColor.r,fragColor.g,fragColor.b);
	
	float r = floor(texColor.r * float(8)) / float(8);
	float g = floor(texColor.g * float(8)) / float(8);
	float b = floor(texColor.b * float(8)) / float(8);
	fragColor = vec4(r, g, b, 1.0);
}
void shader_DoG(int a){		
	int halfWidth = int(ceil(2.0 * 2.8f)); 
	vec2 sum = vec2(0.0); 
	vec2 norm = vec2(0.0); 	
	for (int i = -halfWidth; i <= halfWidth; ++i) {	
		for (int j = -halfWidth; j <= halfWidth; ++j) {	
			float d = length(vec2(i, j)); 
			vec2 kernel = vec2(exp(-d * d / (2.0 * 2.0f * 2.0f)),exp(-d * d / (2.0 * 2.8f * 2.8f))); 
			vec4 texColor = texture(tex, vertexData.texcoord + vec2(i, j) / img_size); 
			vec2 L = vec2(0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b); 				
			norm += 2.0 * kernel; 
			sum += kernel * L; 
		}
	}
	sum /= norm; 
	float H = 100.0 * (sum.x - 0.99f * sum.y); 
	float edge = (H > 0.0) ? 1.0 : 2.0 * smoothstep(-2.0, 2.0, 3.4f * H);
	if(a == 0)	
		fragColor = vec4(edge, edge, edge, 1.0); 		
	else
		fragColor *= vec4(edge, edge, edge, 1.0); 	
}
void shader_AverageFilter(){
	fragColor = vec4(0,0,0,0);
	int n = 0;
	int half_size = 3;
	for (int i = -half_size; i <= half_size; ++i) {
		for (int j = -half_size; j <= half_size; ++j) {
			vec4 texColor = texture(tex,vertexData.texcoord + vec2(i, j) / img_size);
			fragColor += texColor;
			n++;
		}
	}
	fragColor /= n;
}
void shader_Abstraction(){
	shader_AverageFilter();
	shader_Quantization(1);
	shader_DoG(1);
}

void shader_ThresholdDither(){
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	float grayColor = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
	if(grayColor < 0.5)
		grayColor = 0;
	else
		grayColor = 1;
	fragColor = vec4(grayColor,grayColor,grayColor, 1.0);
}
void gold(){
	fragColor = vec4(1,215.0/255.0,0, 1.0);
}
void main()
{	
	switch (shaderNumber)
	{
		case(0):
		{
			Shader_Normal();
			break;
		}
		case(1):
		{				
			Shader_Gray();			
			break;
		}
		case(2):
		{
			shader_Quantization(0);
			break;
		}
		case(3):
		{
			shader_DoG(0);
			break;
		}
		case(4):
		{
			shader_Abstraction();
			break;
		}
		case(5):
		{
			shader_AverageFilter();
			break;
		}
		case(6):
		{
			shader_ThresholdDither();
			break;
		}
			
	}

}

