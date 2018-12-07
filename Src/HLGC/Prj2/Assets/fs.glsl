#version 410

in vec2 coord;
out vec4 fragColor; 
uniform sampler2D tex;
uniform int effect;
uniform int type;
uniform float time;
vec2 img_size = vec2(800., 340.);
const vec2 iResolution = vec2(800., 600.);
flat in vec4 color;

void Normal(){
	vec4 texColor = vec4(1,1,1,1);
	if(type == 0)
		texColor = texture(tex,coord);	
	else if(type == 1)
		texColor = texture(tex,gl_PointCoord)*color;	
	fragColor = texColor;		
}

void Gray(){	
	
	vec4 texColor = texture(tex,coord);	
	float grayColor = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
	fragColor = vec4(grayColor,grayColor,grayColor,texColor.a);
}
void Quantization(int a){
	vec4 texColor;
	if(a == 0)	
		texColor = texture(tex,coord);	
	else	
		texColor =  fragColor;
	
	float r = floor(texColor.r * float(8)) / float(8);
	float g = floor(texColor.g * float(8)) / float(8);
	float b = floor(texColor.b * float(8)) / float(8);
	fragColor = vec4(r, g, b, texColor.a);
}
void DoG(int a){		
	int halfWidth = int(ceil(2.0 * 2.8f)); 
	vec2 sum = vec2(0.0); 
	vec2 norm = vec2(0.0); 	
	for (int i = -halfWidth; i <= halfWidth; ++i) {	
		for (int j = -halfWidth; j <= halfWidth; ++j) {	
			float d = length(vec2(i, j)); 
			vec2 kernel = vec2(exp(-d * d / (2.0 * 2.0f * 2.0f)),exp(-d * d / (2.0 * 2.8f * 2.8f))); 
			vec4 texColor = texture(tex, coord + vec2(i, j) / img_size); 
			vec2 L = vec2(0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b); 				
			norm += 2.0 * kernel; 
			sum += kernel * L; 
		}
	}
	sum /= norm; 
	float H = 100.0 * (sum.x - 0.99f * sum.y); 
	float edge = (H > 0.0) ? 1.0 : 2.0 * smoothstep(-2.0, 2.0, 3.4f * H);
	if(a == 0)	
		fragColor = vec4(edge, edge, edge, fragColor.a); 		
	else
		fragColor *= vec4(edge, edge, edge, fragColor.a); 	
}
void TriangleFilter(){
 vec2 coord1 = coord + vec2(-3.0, -3.0) / img_size;
 vec2 coord2 = coord + vec2(-3.0, 3.0) / img_size;
 vec2 coord3 = coord + vec2(3.0, -3.0) / img_size;
 vec2 coord4 = coord + vec2(3.0, 3.0) / img_size;

 vec4 s1 = vec4(texture(tex, coord1));
 vec4 s2 = vec4(texture(tex, coord2));
 vec4 s3 = vec4(texture(tex, coord3));
 vec4 s4 = vec4(texture(tex, coord4));

 vec4 tmp1 = mix(s1, s3, coord.x);
 vec4 tmp2 = mix(s2, s4, coord.x);

 fragColor = mix(tmp1, tmp2, coord.y);
}
void ThresholdDither(){
	vec4 texColor = texture(tex,coord);
	float grayColor = 0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b;
	if(grayColor < 0.5)
		grayColor = 0;
	else
		grayColor = 1;
	fragColor = vec4(grayColor,grayColor,grayColor, texColor.a);
}
void FD(){
	const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
	const vec2 TexSize = vec2(50.0, 50.0);
	const vec4 bkColor = vec4(0.5, 0.5, 0.5, 0.0);  
    vec2 upLeftUV = vec2(coord.x-1.0/TexSize.x, coord.y-1.0/TexSize.y);
    vec4 curColor = texture(tex, coord);
    vec4 upLeftColor = texture(tex, upLeftUV);
    vec4 delColor = curColor - upLeftColor;
    float luminance = dot(delColor.rgb, W);
    fragColor = vec4(vec3(luminance), curColor.a) + bkColor;
}
/*-----------------------------�o���S��---------------------------------*/
float length2(vec2 p) { return dot(p, p); }

float noise(vec2 p)
{
	return fract(sin(fract(sin(p.x) * (4313.13311)) + p.y) * 3131.0011);
}

float worley(vec2 p) 
{
	float d = 1e30;
	for (int xo = -1; xo <= 1; ++xo)
	for (int yo = -1; yo <= 1; ++yo) 
	{
		vec2 tp = floor(p) + vec2(xo, yo);
		d = min(d, length2(p - tp - vec2(noise(tp))));
	}
	return 3.*exp(-4.*abs(2.*d - 1.));
}

float fworley(vec2 p) 
{
	return sqrt(sqrt(sqrt(
		pow(worley(p + time*0.001), 2.) *
		worley(p*2. + 1.3 + time*0.001*.5) *
		worley(p*4. + 2.3 + time*0.001*.25) *
		worley(p*8. + 3.3 + time*0.001*.125) *
		worley(p*32. + 4.3 + time*0.001*.125) *
		sqrt(worley(p * 64. + 5.3 + time*0.001 * .0625)) *
		sqrt(sqrt(worley(p * 128. + 7.3))))));
}
void Phosgene() 
{
	vec4 texColor = texture(tex,coord);	
	fragColor = texColor;
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	float t = fworley(uv * iResolution.xy / 600.);
	t *= exp(-length2(abs(2.*uv - 1.)));
	float r = length(abs(2.*uv - 1.) * iResolution.xy);
	fragColor += vec4(t * vec3(1.8, 1.8*t, .1 + pow(t, 2.-t)), 0.);
}
/*--------------------------------��------------------------------------*/
#define _SnowflakeAmount 300  
#define _BlizardFactor 0.25          
float rnd(float x) {  return fract(sin(dot(vec2(x + 47.49, 38.2467 / (x + 2.3)),vec2(12.9898, 78.233))) * (43758.5453));}
float drawCircle(vec2 uv, vec2 center, float radius)
{
   return 1.0 - smoothstep(0.0, radius, length(uv - center));
}

void snow(){
   vec2 uv =  gl_FragCoord.xy  /iResolution.x;
   vec4 color = texture(tex,coord);  
   fragColor = color;  
   
   float j;   
   for (int i = 0; i < _SnowflakeAmount; i++)
   {
      j = float(i);      
      float speed = 0.3 + rnd(cos(j)) * (0.7 + 0.5 * cos(j / (float(_SnowflakeAmount) * 0.25)));           
      vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(time*0.001 + sin(j)),mod(rnd(j) - speed * (time * 0.001 * (0.1 + _BlizardFactor)),0.95));
      fragColor += vec4(0.9 * drawCircle(uv, center , 0.001 + speed * 0.008 ));
	  
   }
   
}
/*---------------------------------����------------------------------------*/

float polygonDistance(vec2 p, float radius, float angleOffset, int sideCount) {
	float a = atan(p.x, p.y)+ angleOffset;
	float b = 6.28319 / float(sideCount);
	return cos(floor(.5 + a / b) * b - a) * length(p) - radius;
}

#define HASHSCALE1 443.8975
float hash11(float p) 
{
	vec3 p3  = fract(vec3(p) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

#define HASHSCALE3 vec3(.1031, .1030, .0973)
vec2 hash21(float p) 
{
	vec3 p3 = fract(vec3(p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract(vec2((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y));
}

void NebulaSmoke()
{
	vec4 texColor = texture(tex,coord);	
	fragColor = texColor;	
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
    
    vec3 subColor = vec3(0.4, 0.8, 0.2); 
    vec3 addColor = vec3(0.3, 0.2, 0.1);
    
	fragColor += vec4(vec3(1.0) - accum * subColor + addColor, 0.0);
}
/*-----------------------------�������--------------------------------*/
#define tau 6.2831853

mat2 makem2(in float theta){float c = cos(theta);float s = sin(theta);return mat2(c,-s,s,c);}
float noisess( in vec2 x ){return texture(tex, x*.01).x;}

float fbmm(in vec2 p)
{	
	float z=2.;
	float rz = 0.;
	vec2 bp = p;
	for (float i= 1.;i < 6.;i++)
	{
		rz+= abs((noisess(p)-0.5)*2.)/z;
		z = z*2.;
		p = p*2.;
	}
	return rz;
}

float dualfbm(in vec2 p)
{
    
	vec2 p2 = p*.7;
	vec2 basis = vec2(fbmm(p2-time*0.0001*1.6),fbmm(p2+time*0.0001*1.7));
	basis = (basis-.5)*.2;
	p += basis;	
	return fbmm(p*makem2(time*0.0001*0.2));
}

float circc(vec2 p) 
{
	float r = length(p);
	r = log(sqrt(r));
	return abs(mod(r*4.,tau)-3.14)*3.+.2;

}

void PurpleHalo()
{   
    vec4 texColor = texture(tex,coord);	
	fragColor = texColor;	
	vec2 p = gl_FragCoord.xy / iResolution.xy-0.5;
	p.x *= iResolution.x/iResolution.y;
	p*=4.;
	
    float rz = dualfbm(p);	
	
	p /= exp(mod(time*0.0001*10.,3.14159));
	rz *= pow(abs((0.1-circc(p))),.9);	
	
	vec3 col = vec3(.2,0.1,0.4)/rz;
	col=pow(abs(col),vec3(.99));
	fragColor += vec4(col,0.0);
}
/*-----------------------------------------------------*/
void main(void)
{				
	switch (effect)
	{
		case(0):
		{
			Normal();			
			break;
		}
		case(1):
		{					
			Gray();					
			break;
		}
		case(2):
		{
			Quantization(0);
			break;
		}
		case(3):
		{
			DoG(0);
			break;
		}		
		case(4):
		{
			TriangleFilter();
			break;
		}
		case(5):
		{
			ThresholdDither();
			break;
		}		
		case(6):{		
			FD();
			break;
		}		
		case(7):{		
			Phosgene();
			break;
		}
		case(8):{
			snow();
			break;
		}		
		case(9):{		
			NebulaSmoke();
			break;
		}		
		case(10):{		
			PurpleHalo();
			break;
		}				
	}	
}
