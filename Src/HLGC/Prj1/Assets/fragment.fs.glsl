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

vec2 img_size = vec2(400, 400); 
float count = 1.1;

float rand(float x) { return fract(sin(x) * 4358.5453); }
float rand(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 3758.5357); }
float rnd(float x) {  return fract(sin(dot(vec2(x + 47.49, 38.2467 / (x + 2.3)),vec2(12.9898, 78.233))) * (43758.5453));}

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

void mosaic()
{
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

/*---------------------------------------------------------------------*/
float invader(vec2 p, float n)
{
   p.x = abs(p.x);
   p.y = -floor(p.y - 5.0);   
   return step(p.x, 2.0) * step(1.0, floor(mod(n/(exp2(floor(p.x + p.y*3.0))),2.0)));
}

void Textspeak(){
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);	

	vec3 color = vec3(0.2, 0.42, 0.68); 	
	float width = 8.0;

	vec2 p = gl_FragCoord.xy;
    vec2 uv = p / iResolution.xy - 0.5;
    
    float id1 = rand(floor(p.x / width));             
    float id2 = rand(floor((p.x - 1.0) / width));   
    
    float a = 0.3*id1;                                       
    a += 0.1*step(id2, id1 - 0.08);                    
    a -= 0.1*step(id1 + 0.08, id2);                     
    a -= 0.3*smoothstep(0.0, 0.7, length(uv));   
 
   
    p.y += 20.0*time*0.002;
    float r = rand(floor(p/8.0));
    float inv = invader(mod(p,8.0)-4.0, 809999.0*r);
    a += (0.06 + max(0.0, 0.2*sin(10.0*r*time*0.002))) * inv * step(id1, .3);            
    fragColor += vec4(color+a, 1.0);
}
/*----------------------------------------------------------------*/
#define _SnowflakeAmount 300  
#define _BlizardFactor 0.25          

float drawCircle(vec2 uv, vec2 center, float radius)
{
   return 1.0 - smoothstep(0.0, radius, length(uv - center));
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
      fragColor += vec4(0.9 * drawCircle(uv, center , 0.001 + speed * 0.008 ));
	  
   }
   
}
vec2 rotateUV(vec2 uv, float rotation)
{
    float mid = 0.5;
    return vec2(
        cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
        cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}
void gold_particle(){
   vec2 uv =  gl_FragCoord.xy  /iResolution.y;
   uv = rotateUV(uv,180);
   vec3 color = texture(tex,vertexData.texcoord).rgb;
   
   fragColor = vec4(color, 1.0);
   float j;   
   for (int i = 0; i < _SnowflakeAmount; i++)
   {
      j = float(i);      
      float speed = 0.3 + rnd(cos(j)) * (0.7 + 0.5 * cos(j / (float(_SnowflakeAmount) * 0.25)));
      vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(time*0.001 + sin(j)),mod(rnd(j) - speed * (time * 0.001 * (0.1 + _BlizardFactor)),0.95));      
	  fragColor +=(vec4(255.0 / 255.0 , 215.0 / 255.0, 0 / 255.0,1.0) * vec4(0.9 * drawCircle(uv, center , 0.001 + speed * 0.008)));
   }
}
/*-----------------------------------------------------------------------------*/
vec2 hash( vec2 p ) 
{ 
   p=vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3))); 
   
   return fract(sin(p)*18.5453);
}
vec2 voronoi( in vec2 x )
{
    vec2 n = floor( x );              
    vec2 f = fract( x );              

   vec3 m = vec3( 8. );               
   
   
    for( int j=-1; j<=1; j++ )
    {
       for( int i=-1; i<=1; i++ )
       {
           vec2  g = vec2( float(i), float(j) );            
           vec2  o = hash( n + g );                       
           vec2  r = g - f + (0.5+0.5*sin(time*0.001+6.2831*o));                          
           float d = dot( r, r );           
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
    vec2 c = voronoi( (14.0+6.0*sin(0.2*time*0.001))*p );    
    vec3 col = 0.5 + 0.5*cos( c.y*6.2831 + vec3(0.0,1.0,2.0) );        
    col *= clamp(1.0 - 0.6*c.x*c.x, 0.0, 1.0);
    col -= (1.0-smoothstep( 0.05, 0.06, c.x));                   
    fragColor += vec4( col, 0.0 );
}
/*-----------------------------------------------------------------------------*/
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
    
    vec3 subColor = vec3(0.4, 0.8, 0.2); 
    vec3 addColor = vec3(0.3, 0.2, 0.1);
    
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
void Phosgene() {
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	float t = fworley(uv * iResolution.xy / 600.);
	t *= exp(-length2(abs(2.*uv - 1.)));
	float r = length(abs(2.*uv - 1.) * iResolution.xy);
	fragColor += vec4(t * vec3(1.8, 1.8*t, .1 + pow(t, 2.-t)), 1.);
}

/*-----------------------------------------------------------------------------*/
vec2 cmul(vec2 z0, vec2 z1)
{
    return vec2(z0.x * z1.x - z0.y * z1.y, z0.x * z1.y + z0.y * z1.x);
}

vec2 cdiv(vec2 z0, vec2 z1)
{
    vec2 z1conj = vec2(z1.x, -z1.y);
    return cmul(z0, z1conj) / cmul(z1, z1conj).x;
}

vec4 hermite(mat4 m, float t)
{
    float t2 = t * t, t3 = t2 * t;
    return m * vec4(2.0 * t3 - 3.0 * t2 + 1.0, t3 - 2.0 * t2 + t, -2.0 * t3 + 3.0 * t2, t3 - t2);
}

vec3 catmullRom(mat4 m, float t)
{
    if(t > m[2].w)
        m = mat4(m[1], m[2], m[3], m[3] * 2.0 - m[2]);
    else if(t > m[1].w)
        m = mat4(m[0], m[1], m[2], m[3]);
    else
        m = mat4(m[0] * 2.0 - m[1], m[0], m[1], m[2]);

    t = (t - m[1].w) / (m[2].w - m[1].w);
    m = m * mat4(0.0, 1.0, 0.0, 0.0, -0.5, 0.0, 0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, -0.5, 0.0, 0.5);
    return hermite(m, t).xyz;
}

float pattern(vec2 p, int n, vec2 o)
{
    p *= 0.5;
    vec2 t = vec2(cos(1.04719755), sin(1.04719755)) * 2.0;
    float m = 1.0;
    for(int i = 0; i < 8; i += 1)
    {
        if(i < n)
        {
            p = cdiv(vec2(1.0, 0.0), cmul(abs(fract(p) - vec2(0.5)), t));
            m = max(m, length(p) / 2.0);
        }
    }
    return 1.1 - exp(-distance(p, o) / m) * 1.6;
}

void Pulsations()
{
	
    vec2 uv = gl_FragCoord.xy / iResolution.y * 0.7;
    vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);

    mat4 m = mat4( vec3(0.1, 0.1, 0.2), 0.2,  vec3(0.2, 0.3, 0.42), 0.7,   
                vec3(1.0, 0.9, 0.5), 0.92, vec3(0.2, 0.3, 0.42), 1.0);

    for(int i = 0; i < 8; i+=1)
    {
        vec2 c = uv + float(i) * 5e-5;
        float p = pattern(c, 5, vec2(cos(time*0.005 + c.y * 15.0) * 0.5, 0.0)) * 0.75 +
            pattern(c.yx, 6, vec2(0.0, sin(time*0.005 * 0.5) * 0.5)) * 0.25 +
            pattern(c.yx * 2.0, 7, vec2(0.0, sin(time*0.005 + c.x * 8.0))) * 0.2;
        fragColor.rgb += catmullRom(m, clamp(p * 0.9, 0.0, 1.0));
    }
    fragColor.rgb /= 8.0;
}
/*-----------------------------------------------------------------------------*/
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
    vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);	
	vec2 p = gl_FragCoord.xy / iResolution.xy-0.5;
	p.x *= iResolution.x/iResolution.y;
	p*=4.;
	
    float rz = dualfbm(p);	
	
	p /= exp(mod(time*0.0001*10.,3.14159));
	rz *= pow(abs((0.1-circc(p))),.9);	
	
	vec3 col = vec3(.2,0.1,0.4)/rz;
	col=pow(abs(col),vec3(.99));
	fragColor += vec4(col,1.);
}
/*----------------------------------------------------------------------------*/

vec3 hsv2rgb (in vec3 hsv) {
	hsv.yz = clamp (hsv.yz, 0.0, 1.0);
	return hsv.z * (1.0 + 0.5 * hsv.y * (cos (2.0 * 3.14159 * (hsv.x + vec3 (0.0, 2.0 / 3.0, 1.0 / 3.0))) - 1.0));
}

float randd (in vec2 seed) {
	return fract (sin (dot (seed, vec2 (12.9898, 78.233))) * 137.5453);
}

void DanceFloor() {
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);
	vec2 frag = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.y;
	frag *= 1.0 - 0.2 * cos (frag.yx) * sin (3.14159 * 0.5 * texture (tex, vec2 (0.0)).x);
	frag *= 5.0;
	float random = randd (floor (frag));
	vec2 black = smoothstep (1.0, 0.8, cos (frag * 3.14159 * 2.0));
	vec3 color = hsv2rgb (vec3 (random, 1.0, 1.0));
	color *= black.x * black.y * smoothstep (1.0, 0.0, length (fract (frag) - 0.5));
	color *= 0.5 + 0.5 * cos (random + random * time*0.001 + time*0.001 + 3.14159 * 0.5 * texture (tex, vec2 (0.7)).x);
	fragColor += vec4 (color, 1.0);
}
/*-------------------------------------------------------------------------*/
mat2 m2 = mat2( 0.80,  0.60, -0.60,  0.80 );
float grid(vec2 p)
{
	float s = sin(p.x)*cos(p.y);
	return s;
}

float flow(in vec2 p)
{
	float z=2.;
	float rz = 0.;
	vec2 bp = p;
	for (float i= 1.;i < 7.;i++ )
	{
		bp += time*0.0005*1.5;
		vec2 gr = vec2(grid(p*3.-time*0.0005*2.),grid(p*3.+4.-time*0.0005*2.))*0.4;
		gr = normalize(gr)*0.4;
		gr *= makem2((p.x+p.y)*.3+time*0.0005*10.);
		p += gr*0.5;
		
		rz+= (sin(noise(p)*8.)*0.5+0.5) /z;
		
		p = mix(bp,p,.5);
		z *= 1.7;
		p *= 2.5;
		p*=m2;
		bp *= 2.5;
		bp*=m2;
	}
	return rz;	
}

float spiral(vec2 p,float scl) 
{
	float r = length(p);
	r = log(r);
	float a = atan(p.y, p.x);
	return abs(mod(scl*(r-2./scl*a),tau)-1.)*2.;
}

void YellowWhirlwind() 
{
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);
	vec2 p = gl_FragCoord.xy / iResolution.xy-0.5;
	p.x *= iResolution.x/iResolution.y;
	p*= 3.;
	float rz = flow(p);
	p /= exp(mod(time*0.0005*3.,2.1));
	rz *= (6.-spiral(p,3.))*.9;
	vec3 col = vec3(.2,0.07,0.01)/rz;
	col=pow(abs(col),vec3(1.01));
	fragColor += vec4(col,1.0);
}
/*-----------------------------------------------------------------------*/
const vec3 BackColor	= vec3(0.0, 0.4, 0.58);
const vec3 CloudColor	= vec3(0.18,0.70,0.87);


float Func(float pX)
{
	return 0.6*(0.5*sin(0.1*pX) + 0.5*sin(0.553*pX) + 0.7*sin(1.2*pX));
}


float FuncR(float pX)
{
	return 0.5 + 0.25*(1.0 + sin(mod(40.0*pX, tau)));
}


float Layer(vec2 pQ, float pT)
{
	vec2 Qt = 3.5*pQ;
	pT *= 0.5;
	Qt.x += pT;

	float Xi = floor(Qt.x);
	float Xf = Qt.x - Xi -0.5;

	vec2 C;
	float Yi;
	float D = 1.0 - step(Qt.y,  Func(Qt.x));
	
	Yi = Func(Xi + 0.5);
	C = vec2(Xf, Qt.y - Yi ); 
	D =  min(D, length(C) - FuncR(Xi+ pT/80.0));
	
	Yi = Func(Xi+1.0 + 0.5);
	C = vec2(Xf-1.0, Qt.y - Yi ); 
	D =  min(D, length(C) - FuncR(Xi+1.0+ pT/80.0));
	
	Yi = Func(Xi-1.0 + 0.5);
	C = vec2(Xf+1.0, Qt.y - Yi ); 
	D =  min(D, length(C) - FuncR(Xi-1.0+ pT/80.0));

	return min(1.0, D);
}

void ToonCloud()
{
	vec3 texColor = texture(tex,vertexData.texcoord).rgb;	
	fragColor = vec4(texColor, 1.0);	
	vec2 UV = 2.0*(gl_FragCoord.xy - iResolution.xy/2.0) / min(iResolution.x, iResolution.y);	
	vec3 Color= BackColor;

	for(float J=0.0; J<=1.0; J+=0.2)
	{		
		float Lt =  time*0.001*(0.5  + 2.0*J)*(1.0 + 0.1*sin(226.0*J)) + 17.0*J;
		vec2 Lp = vec2(0.0, 0.3+1.5*( J - 0.5));
		float L = Layer(UV + Lp, Lt);
		
		float Blur = 4.0*(0.5*abs(2.0 - 5.0*J))/(11.0 - 5.0*J);

		float V = mix( 0.0, 1.0, 1.0 - smoothstep( 0.0, 0.01 +0.2*Blur, L ) );
		vec3 Lc=  mix( CloudColor, vec3(1.0), J);

		Color = mix(Color, Lc,  V);
	}

	fragColor += vec4(Color, 1.0);
}
/*------------------------------------------------------------------------*/
void TriangleFilter(){
 vec2 coord1 = vertexData.texcoord + vec2(-3.0, -3.0) / img_size;
 vec2 coord2 = vertexData.texcoord + vec2(-3.0, 3.0) / img_size;
 vec2 coord3 = vertexData.texcoord + vec2(3.0, -3.0) / img_size;
 vec2 coord4 = vertexData.texcoord + vec2(3.0, 3.0) / img_size;

 vec4 s1 = vec4(texture(tex, coord1));
 vec4 s2 = vec4(texture(tex, coord2));
 vec4 s3 = vec4(texture(tex, coord3));
 vec4 s4 = vec4(texture(tex, coord4));

 vec4 tmp1 = mix(s1, s3, vertexData.texcoord.x);
 vec4 tmp2 = mix(s2, s4, vertexData.texcoord.x);

 fragColor = mix(tmp1, tmp2, vertexData.texcoord.y);
}
/*------------------------------------------------------------------------*/
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
		case(7):
		{
		//Mosaic
			mosaic();
			break;		
		}		
		case(8):{
		//Emboss
			FD();
			break;
		}
		case(9):{
			Textspeak();
			break;
		}
		case(10):{
			snow();
			break;
		}
		case(11):{		
		//Voronoi
			VoronoiDiagram();
			break;
		}
		
		case(12):{
		//Nebula smoke
			NebulaSmoke();
			break;
		}
		case(13):{		
			Phosgene();
			break;
		}
		
		case(14):{		
			PurpleHalo();
			break;
		}
		case(15):{		
			DanceFloor();
			break;
		}
		case(16):{		
			YellowWhirlwind();
			break;
		}
		case(17):{		
			ToonCloud();
			break;
		}
		case(18):{		
			gold_particle();
			break;
		}
		case(19):{		
			TriangleFilter();
			break;
		}
		case(20):{		
			Pulsations();
			break;
		}
		case(21):{
			//Origin
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;				
			fragColor = vec4(texColor, 1.0);	
			break;
		}
		case(22):{
			//white		
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;				
			fragColor = vec4(texColor.r + 135.0/255.0,texColor.g + 19.0/255.0,texColor.b + 34.0/255.0, 1.0);	
			break;
		}
		case(23):{			
			//pink
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.g,texColor.r,texColor.b, 1.0);	
			break;
		}
		case(24):{
			//green			
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.r,texColor.g,texColor.r, 1.0);	
			break;
		}
		case(25):{
			//purple
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;						
			fragColor = vec4(texColor.r,texColor.g/7,texColor.b, 1.0);	
			break;
		}
		case(26):{			
			//black			
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;
			fragColor = vec4(texColor.r - 120.0/255.0,texColor.g - 236.0/255.0,texColor.b - 221.0/255.0, 1.0);	
			break;
		}
		case(27):{
			//blond
			vec3 texColor = texture(tex,vertexData.texcoord).rgb;
			fragColor = vec4(texColor.r + 135.0/255.0,texColor.g + 19.0/255.0,texColor.b - 221.0/255.0, 1.0);				
			break;
		}		
	}
}

