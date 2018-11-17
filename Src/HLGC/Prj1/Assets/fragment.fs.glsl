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
 
 #define _SnowflakeAmount 400  // 雪花数
#define _BlizardFactor 0.25          // 风的大小

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


const vec2 iResolution = vec2(800., 800.);


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
   vec2 uv =  gl_FragCoord.xy  /iResolution.x;
   vec3 color = texture(tex,vertexData.texcoord).rgb;  
   fragColor = vec4(color, 1.0);   
   float j;   
   for (int i = 0; i < _SnowflakeAmount; i++)
   {
      j = float(i);      
      float speed = 0.3 + rnd(cos(j)) * (0.7 + 0.5 * cos(j / (float(_SnowflakeAmount) * 0.25)));           
      vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(time*0.001 + sin(j)),mod(rnd(j) - speed * (time * 0.001 * (0.1 + _BlizardFactor)),0.95));
      fragColor += vec4(0.9 * drawCircle(uv, center , 0.001 + speed * 0.008)); 
   }
}
void mosaic(){
	const vec2 TexSize = vec2(800.0, 800.0);	
	const vec2 mosaicSize = vec2(30.0, 30.0);
	vec2 intXY = vec2(vertexData.texcoord.x*TexSize.x, vertexData.texcoord.y*TexSize.y);
    vec2 XYMosaic = vec2(floor(intXY.x/mosaicSize.x)*mosaicSize.x, floor(intXY.y/mosaicSize.y)*mosaicSize.y);
    vec2 UVMosaic = vec2(XYMosaic.x/TexSize.x, XYMosaic.y/TexSize.y);
    vec4 color = texture(tex, UVMosaic);
     fragColor  = color;
}
void FD(){
	const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
	const vec2 TexSize = vec2(50.0, 50.0);
	const vec4 bkColor = vec4(0.5, 0.5, 0.5, 1.0);  
    vec2 upLeftUV = vec2(vertexData.texcoord.x-1.0/TexSize.x, vertexData.texcoord.y-1.0/TexSize.y);
    vec4 curColor = texture(tex, vertexData.texcoord);
    vec4 upLeftColor = texture(tex, upLeftUV);
    vec4 delColor = curColor - upLeftColor;
    float luminance = dot(delColor.rgb, W);
    fragColor = vec4(vec3(luminance), 0.0) + bkColor;
}


/*-----------------------------------------------------------------------------*/
vec2 hash( vec2 p ) 
{ 
   p=vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3))); 
   
   return fract(sin(p)*18.5453);
}

// return distance, and cell id
vec2 voronoi( in vec2 x )
{
    vec2 n = floor( x );              // cell(n)
    vec2 f = fract( x );              // 当前像素在cell space的坐标

   vec3 m = vec3( 8. );               // 影响每个cell的大小，影响背景颜色
   
   // 遍历相邻的9个cell
    for( int j=-1; j<=1; j++ )
    {
       for( int i=-1; i<=1; i++ )
       {
           vec2  g = vec2( float(i), float(j) );  // 临近的 cell id offset
           // n+g 临近的 cell(n+g) 的随机像素坐标 o (cell space)
           vec2  o = hash( n + g );   // 影响cell的颜色
           
           // ❤
           vec2  r = g - f + (0.5+0.5*sin(time+6.2831*o));
           //vec2  r = g - f + o;     // cell(n+g)[o] - cell(n)[f] 
          
           // ❤
           float d = dot( r, r );
 
           // 保存更小的d
           if( d<m.x )
           {
              m = vec3( d, o );
           }
       }
    }
    return vec2( sqrt(m.x), m.y+m.z );
}
void VoronoiDiagram(){
vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);	
vec2 iResolution = vec2(512., 512.);
   vec2 p = gl_FragCoord.xy/max(iResolution.x,iResolution.y);
    
   // computer voronoi patterm
   vec2 c = voronoi( (14.0+6.0*sin(0.2*time))*p );
   
   // colorize
   vec3 col = 0.5 + 0.5*cos( c.y*6.2831 + vec3(0.0,1.0,2.0) ); // cell的随机颜色
     
   col *= clamp(1.0 - 0.6*c.x*c.x, 0.0, 1.0);
   col -= (1.0-smoothstep( 0.05, 0.06, c.x));                  // 画Voronoi的点集
   
   fragColor += vec4( col, 0.0 );

}
/*-----------------------------------------------------------------------------*/
float polygonDistance(vec2 p, float radius, float angleOffset, int sideCount) {
	float a = atan(p.x, p.y)+ angleOffset;
	float b = 6.28319 / float(sideCount);
	return cos(floor(.5 + a / b) * b - a) * length(p) - radius;
}

#define HASHSCALE1 443.8975
float hash11(float p) // assumes p in ~0-1 range
{
	vec3 p3  = fract(vec3(p) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

#define HASHSCALE3 vec3(.1031, .1030, .0973)
vec2 hash21(float p) // assumes p in larger integer range
{
	vec3 p3 = fract(vec3(p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract(vec2((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y));
}

void NebulaSmoke()
{
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);	
	vec2 uv = vec2(0.5) - (gl_FragCoord.xy / iResolution.xy);
    uv.x *= iResolution.x / iResolution.y;
    
    float accum = 0.;
    for(int i = 0; i < 83; i++) {
        float fi = float(i);
        float thisYOffset = mod(hash11(fi * 0.017) * (time*0.005 + 19.) * 0.2, 4.0) - 2.0;
        vec2 center = (hash21(fi) * 2. - 1.) * vec2(1.1, 1.0) - vec2(0.0, thisYOffset);
        float radius = 0.5;
        vec2 offset = uv - center;
        float twistFactor = (hash11(fi * 0.0347) * 2. - 1.) * 1.9;
        float rotation = 0.1 + time*0.005 * 0.2 + sin(time*0.005 * 0.1) * 0.9 + (length(offset) / radius) * twistFactor;
        accum += pow(smoothstep(radius, 0.0, polygonDistance(uv - center, 0.1 + hash11(fi * 2.3) * 0.2, rotation, 5) + 0.1), 3.0);
    }
    
    vec3 subColor = vec3(0.4, 0.8, 0.2); //vec3(0.4, 0.2, 0.8);
    vec3 addColor = vec3(0.3, 0.2, 0.1);//vec3(0.3, 0.1, 0.2);
    
	fragColor += vec4(vec3(1.0) - accum * subColor + addColor, 1.0);
}
/*-----------------------------------------------------------------------------*/
float length2(vec2 p) { return dot(p, p); }

float noise(vec2 p){
	return fract(sin(fract(sin(p.x) * (4313.13311)) + p.y) * 3131.0011);
}

float worley(vec2 p) {
	float d = 1e30;
	for (int xo = -1; xo <= 1; ++xo)
	for (int yo = -1; yo <= 1; ++yo) {
		vec2 tp = floor(p) + vec2(xo, yo);
		d = min(d, length2(p - tp - vec2(noise(tp))));
	}
	return 3.*exp(-4.*abs(2.*d - 1.));
}

float fworley(vec2 p) {
	return sqrt(sqrt(sqrt(
		pow(worley(p + time*0.001), 2.) *
		worley(p*2. + 1.3 + time*0.001*.5) *
		worley(p*4. + 2.3 + time*0.001*.25) *
		worley(p*8. + 3.3 + time*0.001*.125) *
		worley(p*32. + 4.3 + time*0.001*.125) *
		sqrt(worley(p * 64. + 5.3 + time*0.001 * .0625)) *
		sqrt(sqrt(worley(p * 128. + 7.3))))));
}
void Glorious_light() {
vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	float t = fworley(uv * iResolution.xy / 600.);
	t *= exp(-length2(abs(2.*uv - 1.)));
	float r = length(abs(2.*uv - 1.) * iResolution.xy);
	fragColor += vec4(t * vec3(1.8, 1.8*t, .1 + pow(t, 2.-t)), 1.);
}
/*-----------------------------------------------------------------------------*/
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
		case(7):{
		//Mosaic
			mosaic();
		
		}
		break;
		case(8):{
		//Emboss
			FD();
		}
		break;
		case(9):{		
		//Voronoi
			VoronoiDiagram();
		}
		break;
		case(10):{
		//Nebula smoke
			NebulaSmoke();
		}
		break;
		case(11):{
		//Glorious light
			Glorious_light();
		}
		break;
		case(21):{
			//white		
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;				
			fragColor = vec4(texColor.r + 135.0/255.0,texColor.g + 19.0/255.0,texColor.b + 34.0/255.0, 1.0);	
			break;
		}
		case(22):{			
			//pink
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.g,texColor.r,texColor.b, 1.0);	
			break;
		}
		case(23):{
			//green			
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.r,texColor.g,texColor.r, 1.0);	
			break;
		}
		case(24):{
			//purple
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.r,texColor.g/7,texColor.b, 1.0);	
			break;
		}
		case(25):{			
			//black			
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;
			fragColor = vec4(texColor.r - 120.0/255.0,texColor.g - 236.0/255.0,texColor.b - 221.0/255.0, 1.0);	
			break;
		}
		case(26):{
			//blond
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;
			fragColor = vec4(texColor.r + 135.0/255.0,texColor.g + 19.0/255.0,texColor.b - 221.0/255.0, 1.0);				
			break;
		}		
	}
}

