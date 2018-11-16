#version 410
layout(location = 0) out vec4 fragColor;
in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord; //座標
} vertexData;

uniform int shaderNumber; //特效編號
uniform sampler2D tex; //輸入之貼圖 
uniform float time;

vec2 img_size = vec2(500, 500); 
float count = 1.1;

 
vec3 color = vec3(0.2, 0.42, 0.68); // blue 1
//vec3 color = vec3(0.1, 0.3, 0.6); // blue 2
//vec3 color = vec3(0.6, 0.1, 0.3); // red
//vec3 color = vec3(0.1, 0.6, 0.3); // green
 
float width = 8.0;   // n = 512/32 = 16
 
float rand(float x) { return fract(sin(x) * 4358.5453); }
float rand(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 3758.5357); }
 
 #define _SnowflakeAmount 400   // 雪花数
#define _BlizardFactor 0.25           // 风的大小

float rnd(float x)
{
   return fract(
         sin(
               dot(vec2(x + 47.49, 38.2467 / (x + 2.3)),
                     vec2(12.9898, 78.233))) * (43758.5453));
}


float drawCircle(vec2 uv, vec2 center, float radius)
{
   return 1.0 - smoothstep(0.0, radius, length(uv - center));
}



float invader(vec2 p, float n)
{
   p.x = abs(p.x);
   p.y = -floor(p.y - 5.0);
   
   //return step(p.x, 2.0) ;
   //return floor(p.x + p.y*3.0);
   //return n/exp2(floor(p.x + p.y*3.0));
   //return mod(n/exp2(floor(p.x + p.y*3.0)), 2.);
   //return floor(mod(n/exp2(floor(p.x + p.y*3.0)), 2.));
   //return step(1.0, floor(mod(n/(exp2(floor(p.x + p.y*3.0))),2.0)));
   return step(p.x, 2.0) * step(1.0, floor(mod(n/(exp2(floor(p.x + p.y*3.0))),2.0)));
}


const vec2 iResolution = vec2(512., 512.);


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
	fragColor = vec4(255.0 / 255.0 , 215.0 / 255.0, 0 / 255.0, 0.5);
}
void bluemove(){
	vec2 p = gl_FragCoord.xy;
    vec2 uv = p / iResolution.xy - 0.5;
    
    float id1 = rand(floor(p.x / width));             // 影响当前像素（矩形内）的颜色
    float id2 = rand(floor((p.x - 1.0) / width));   // 影响其邻近左侧像素的颜色
    
    float a = 0.3*id1;                                       // 当前矩形的颜色
    a += 0.1*step(id2, id1 - 0.08);                     // 矩形左边界（如果当前像素比左侧淡，则亮边界）
    a -= 0.1*step(id1 + 0.08, id2);                     // 矩形右边界（如果当前像素比左侧深，则亮边暗）
    a -= 0.3*smoothstep(0.0, 0.7, length(uv));   // 渐变效果
 
   
    p.y += 20.0*time;
    float r = rand(floor(p/8.0));
    float inv = invader(mod(p,8.0)-4.0, 809999.0*r);
    a += (0.06 + max(0.0, 0.2*sin(10.0*r*time))) * inv * step(id1, .3);   
    //a += (0.06 + max(0.0, 0.2*sin(10.0*r*time))) * inv;
   
    
   fragColor = vec4(color+a, 1.0);


}
void snow(){
   vec2 uv =  gl_FragCoord.xy / iResolution.x;
   vec3 color = texture(tex,vertexData.texcoord).rgb;
   //vec4 color = texture2D(tex, uv);
   
   //fragColor = vec4(0.808, 0.89, 0.918, 1.0);
   fragColor = vec4(color, 1.0);
   
   float j;
   
   for (int i = 0; i < _SnowflakeAmount; i++)
   {
      j = float(i);
      
      float speed = 0.3 + rnd(cos(j)) * (0.7 + 0.5 * cos(j / (float(_SnowflakeAmount) * 0.25)));
        
      // Test                                              
      //vec2 center = vec2(cos(time + sin(j)),
      //                     cos(time + cos(j)));
       
      vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(time + sin(j)),mod(rnd(j) - speed * (time * 1.5 * (0.1 + _BlizardFactor)),0.95));
      fragColor += vec4(0.9 * drawCircle(uv, center, 0.001 + speed * 0.012)); 
   }
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
			 snow();
			//Shader_Gray();			
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
		case(7):{
			//white		
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;				
			fragColor = vec4(texColor.r + 135.0/255.0,texColor.g + 19.0/255.0,texColor.b + 34.0/255.0, 1.0);	
			break;
		}
		case(8):{			
			//pink
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.g,texColor.r,texColor.b, 1.0);	
			break;
		}
		case(9):{
			//green			
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.r,texColor.g,texColor.r, 1.0);	
			break;
		}
		case(10):{
			//purple
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.r,texColor.g/7,texColor.b, 1.0);	
			break;
		}
		case(11):{			
			//black			
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;				
			fragColor = vec4(texColor.r - 120.0/255.0,texColor.g - 236.0/255.0,texColor.b - 221.0/255.0, 1.0);	
			break;
		}
			
	}

}

