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

float sigma_e = 2.0f; 
float sigma_r = 2.8f; 
float phi = 3.4f; 
float tau = 0.99f; 
float twoSigmaESquared = 2.0 * sigma_e * sigma_e;		
float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;		
int halfWidth = int(ceil(2.0 * sigma_r)); 
vec2 img_size = vec2(1024, 611); 
int nbins = 8; 

void Shader_Normal(){
	//��� Normal
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;
	fragColor = vec4(texColor, 1.0);	
}

void Shader_Gray(){
	//�Ƕ� Gray
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;				
	float grayColor = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
	fragColor = vec4(grayColor,grayColor,grayColor, 1.0);
}

void shader_Quantization(){
	//�q�� Quantization
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;
	float r = floor(texColor.r * float(8)) / float(8);
	float g = floor(texColor.g * float(8)) / float(8);
	float b = floor(texColor.b * float(8)) / float(8);
	fragColor = vec4(r, g, b, 1.0);
}
void shader_DoG(){
	//�����t DoG
	vec2 sum = vec2(0.0); 
	vec2 norm = vec2(0.0); 
	int kernel_count = 0; 
	for (int i = -halfWidth; i <= halfWidth; ++i) {	
			for (int j = -halfWidth; j <= halfWidth; ++j) {	
				float d = length(vec2(i, j)); 
				vec2 kernel = vec2(exp(-d * d / twoSigmaESquared),exp(-d * d / twoSigmaRSquared)); 
				vec4 c = texture(tex, vertexData.texcoord + vec2(i, j) / img_size); 
				vec2 L = vec2(0.299 * c.r + 0.587 * c.g + 0.114 * c.b); 				
				norm += 2.0 * kernel; 
				sum += kernel * L; 
			}
	}
	sum /= norm; 
	float H = 100.0 * (sum.x - tau * sum.y); 
	float edge = (H > 0.0) ? 1.0 : 2.0 * smoothstep(-2.0, 2.0, phi * H); 
	fragColor = vec4(edge, edge, edge, 1.0); 		
}
void shader_Abstraction(){
	//��H�� Abstraction
	fragColor = vec4(0);
	int n = 0;
	int half_size = 3;
	for (int i = -half_size; i <= half_size; ++i) {

		for (int j = -half_size; j <= half_size; ++j) {

			vec4 c = texture(tex, vertexData.texcoord + vec2(i, j) / img_size);
			fragColor += c;
			n++;
		}
	}
	fragColor /= n;

	float r = floor(fragColor.r * float(nbins)) / float(nbins);
	float g = floor(fragColor.g * float(nbins)) / float(nbins);
	float b = floor(fragColor.b * float(nbins)) / float(nbins);
	fragColor = vec4(r, g, b, fragColor.a);
	vec2 sum = vec2(0.0);
	vec2 norm = vec2(0.0);
	int kernel_count = 0;
	for (int i = -halfWidth; i <= halfWidth; ++i) {
		for (int j = -halfWidth; j <= halfWidth; ++j) {
			float d = length(vec2(i, j));
			vec2 kernel = vec2(exp(-d * d / twoSigmaESquared),exp(-d * d / twoSigmaRSquared));
			vec4 c = texture(tex, vertexData.texcoord + vec2(i, j) / img_size);
			vec2 L = vec2(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);

			norm += 2.0 * kernel;
			sum += kernel * L;
		}
	}
	sum /= norm;

	float H = 100.0 * (sum.x - tau * sum.y);
	float edge = (H > 0.0) ? 1.0 : 2.0 * smoothstep(-2.0, 2.0, phi * H);
	fragColor *= vec4(edge, edge, edge, 1.0);

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
			shader_Quantization();
			break;
		}
		case(3):
		{
			shader_DoG();
			break;
		}
		case(4):
		{
			shader_Abstraction();
			break;
		}
			
	}

}
