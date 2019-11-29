#version 430 core
const vec2 iResolution = vec2(800., 800.);

vec2 img_size = vec2(800., 800.);
uniform vec3 camerapos;
in vec2 texturecoord;
in vec3 texturecoord3d;
in vec3 fcolor;
in vec2 uv;
in vec3 vpos;
in vec3 nor;
in vec3 vs_normal;
in vec3 light_vector;
out vec4 fragmentcolor;

vec3 color_diffuse = vec3(0.8,0.8,0.8);
vec3 color_specular = vec3(1.0,1.0,1.0);
vec3 color_ambient = vec3(0.3,0.3,0.3);
vec3 light_position = vec3(30.f,50.f,560.f);

float shininess = 77.f;
float reflectivity = 0.6f;
float eta = 0.66;

uniform int effect;
uniform float alpha;
uniform sampler2D tex;
uniform sampler2D normalmap;
uniform samplerCube texcube;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform float time;
uniform int objType;
uniform bool useTessllation;

float rand(float x) { return fract(sin(x) * 4358.5453); }
float rand(vec2 co) { return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 3758.5357); }
/************Abstraction**************/
void AverageFilter()
{
	fragmentcolor = vec4(0,0,0,0);
	int n = 0;
	int half_size = 3;
	for (int i = -half_size; i <= half_size; ++i) 
	{
		for (int j = -half_size; j <= half_size; ++j) 
		{
			vec4 texColor = texture(tex,texturecoord + vec2(i, j) / img_size);
			fragmentcolor += texColor;
			n++;
		}
	}
	fragmentcolor /= n;
}
void Quantization()
{
	vec4 texColor=fragmentcolor;	
	float r = floor(texColor.r * float(8)) / float(8);
	float g = floor(texColor.g * float(8)) / float(8);
	float b = floor(texColor.b * float(8)) / float(8);
	fragmentcolor = vec4(r, g, b, alpha);
}
void DoG()
{		
	int halfWidth = int(ceil(2.0 * 2.8f)); 
	vec2 sum = vec2(0.0); 
	vec2 norm = vec2(0.0); 	
	for (int i = -halfWidth; i <= halfWidth; ++i) 
	{	
		for (int j = -halfWidth; j <= halfWidth; ++j) 
		{	
			float d = length(vec2(i, j)); 
			vec2 kernel = vec2(exp(-d * d / (2.0 * 2.0f * 2.0f)),exp(-d * d / (2.0 * 2.8f * 2.8f))); 
			vec4 texColor = texture(tex, texturecoord + vec2(i, j) / img_size); 
			vec2 L = vec2(0.299 * texColor.r + 0.587 * texColor.g + 0.114 * texColor.b); 				
			norm += 2.0 * kernel; 
			sum += kernel * L; 
		}
	}
	sum /= norm; 
	float H = 100.0 * (sum.x - 0.99f * sum.y); 
	float edge = (H > 0.0) ? 1.0 : 2.0 * smoothstep(-2.0, 2.0, 3.4f * H);
	fragmentcolor *= vec4(edge, edge, edge, alpha);
}
void Abstraction()
{
	AverageFilter();
	Quantization();
	DoG();
}
/************Abstraction**************/
/***********Nebula smoke**************/
#define HASHSCALE1 443.8975
#define HASHSCALE3 vec3(.1031, .1030, .0973)
float polygonDistance(vec2 p, float radius, float angleOffset, int sideCount) 
{
	float a = atan(p.x, p.y)+ angleOffset;
	float b = 6.28319 / float(sideCount);
	return cos(floor(.5 + a / b) * b - a) * length(p) - radius;
}
float hash11(float p) 
{
	vec3 p3  = fract(vec3(p) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}
vec2 hash21(float p) 
{
	vec3 p3 = fract(vec3(p) * HASHSCALE3);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract(vec2((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y));
}

void NebulaSmoke()
{
	vec2 uv = vec2(0.5) - (gl_FragCoord.xy / iResolution.xy);
    uv.x *= iResolution.x / iResolution.y;
    
    float accum = 0.;
    for(int i = 0; i < 83; i++) 
    {
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
	fragmentcolor += vec4(vec3(1.0) - accum * subColor + addColor, 1.0);    
}
/***********Nebula smoke**************/
/***********Phosgene***************/
float length2(vec2 p) { return dot(p, p); }

float noise(vec2 p)
{
	return fract(sin(fract(sin(p.x) * (4313.13311)) + p.y) * 3131.0011);
}

float worley(vec2 p) 
{
	float d = 1e30;
	for (int xo = -1; xo <= 1; ++xo)
	for (int yo = -1; yo <= 1; ++yo) {
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
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	float t = fworley(uv * iResolution.xy / 600.);
	t *= exp(-length2(abs(2.*uv - 1.)));
	float r = length(abs(2.*uv - 1.) * iResolution.xy);
	fragmentcolor += vec4(t * vec3(1.8, 1.8*t, .1 + pow(t, 2.-t)), 1.);
}
/***********Phosgene***************/
/*********Snow**********/
float rnd(float x) {  return fract(sin(dot(vec2(x + 47.49, 38.2467 / (x + 2.3)),vec2(12.9898, 78.233))) * (43758.5453));}
#define _SnowflakeAmount 300  
#define _BlizardFactor 0.25          

float drawCircle(vec2 uv, vec2 center, float radius)
{
   return 1.0 - smoothstep(0.0, radius, length(uv - center));
}

void snow()
{
   vec2 uv =  gl_FragCoord.xy  / iResolution.x; 
   float j;   
   for (int i = 0; i < _SnowflakeAmount; i++)
   {
      j = float(i);      
      float speed = 0.3 + rnd(cos(j)) * (0.7 + 0.5 * cos(j / (float(_SnowflakeAmount) * 0.25)));           
      vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(time*0.001 + sin(j)),mod(rnd(j) - speed * (time * 0.001 * (0.1 + _BlizardFactor)),0.95));
      fragmentcolor += vec4(0.9 * drawCircle(uv, center , 0.001 + speed * 0.008 ));	  
   }   
}
/************Gold particles**********/
vec2 rotateUV(vec2 uv, float rotation)
{
    float mid = 0.5;
    return vec2(
        cos(rotation) * (uv.x - mid) + sin(rotation) * (uv.y - mid) + mid,
        cos(rotation) * (uv.y - mid) - sin(rotation) * (uv.x - mid) + mid
    );
}
void gold_particle()
{
   vec2 uv =  gl_FragCoord.xy / iResolution.y;
   uv = rotateUV(uv,180);  
   float j;   
   for (int i = 0; i < _SnowflakeAmount; i++)
   {
      j = float(i);      
      float speed = 0.3 + rnd(cos(j)) * (0.7 + 0.5 * cos(j / (float(_SnowflakeAmount) * 0.25)));
      vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(time*0.0005 + sin(j)),mod(rnd(j) - speed * (time * 0.0005 * (0.1 + _BlizardFactor)),0.95));      
	  fragmentcolor +=(vec4(255.0 / 255.0 , 215.0 / 255.0, 0 / 255.0,1.0) * vec4(0.9 * drawCircle(uv, center , 0.001 + speed * 0.008)));
   }
}
/**************************/
/************Purple halo**************/
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
	vec2 p = gl_FragCoord.xy / iResolution.xy-0.5;
	p.x *= iResolution.x/iResolution.y;
	p*=4.;
	
    float rz = dualfbm(p);	
	
	p /= exp(mod(time*0.0001*10.,3.14159));
	rz *= pow(abs((0.1-circc(p))),.9);	
	
	vec3 col = vec3(.2,0.1,0.4)/rz;
	col=pow(abs(col),vec3(.99));
	fragmentcolor += vec4(col,1.);
}
/************Purple halo**************/
/************Dance floor**************/
vec3 hsv2rgb (in vec3 hsv) 
{
	hsv.yz = clamp (hsv.yz, 0.0, 1.0);
	return hsv.z * (1.0 + 0.5 * hsv.y * (cos (2.0 * 3.14159 * (hsv.x + vec3 (0.0, 2.0 / 3.0, 1.0 / 3.0))) - 1.0));
}

float randd (in vec2 seed) 
{
	return fract (sin (dot (seed, vec2 (12.9898, 78.233))) * 137.5453);
}

void DanceFloor() 
{
	vec2 frag = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.y;
	frag *= 1.0 - 0.2 * cos (frag.yx) * sin (3.14159 * 0.5 * texture (tex, vec2 (0.0)).x);
	frag *= 5.0;
	float random = randd (floor (frag));
	vec2 black = smoothstep (1.0, 0.8, cos (frag * 3.14159 * 2.0));
	vec3 color = hsv2rgb (vec3 (random, 1.0, 1.0));
	color *= black.x * black.y * smoothstep (1.0, 0.0, length (fract (frag) - 0.5));
	color *= 0.5 + 0.5 * cos (random + random * time*0.001 + time*0.001 + 3.14159 * 0.5 * texture (tex, vec2 (0.7)).x);
	fragmentcolor += vec4 (color, 1.0);
}
/************Dance floor**************/
/************Gray**************/
void Grayscale()
{	
	float grayValue = fragmentcolor.r*0.3+fragmentcolor.g*0.6+fragmentcolor.b*0.1;
	fragmentcolor = vec4(grayValue,grayValue,grayValue,alpha);
}
/************Gray**************/

/************ThresholdDither**************/
void ThresholdDither()
{
	
	float grayColor = 0.299 * fragmentcolor.r + 0.587 * fragmentcolor.g + 0.114 * fragmentcolor.b;
	if(grayColor < 0.5)
		grayColor = 0;
	else
		grayColor = 1;
	fragmentcolor = vec4(grayColor,grayColor,grayColor, 1.0);
}
/************ThresholdDither**************/
/************Textspeak**************/
float invader(vec2 p, float n)
{
   p.x = abs(p.x);
   p.y = -floor(p.y - 5.0);   
   return step(p.x, 2.0) * step(1.0, floor(mod(n/(exp2(floor(p.x + p.y*3.0))),2.0)));
}
void Textspeak()
{
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
    fragmentcolor += vec4(color+a, 1.0);
}
/************Textspeak**************/
/************VoronoiDiagram**************/
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
void VoronoiDiagram()
{	
	vec2 iResolution = vec2(512., 512.);
    vec2 p = gl_FragCoord.xy/max(iResolution.x,iResolution.y);      
    vec2 c = voronoi( (14.0+6.0*sin(0.2*time*0.001))*p );    
    vec3 col = 0.5 + 0.5*cos( c.y*6.2831 + vec3(0.0,1.0,2.0) );        
    col *= clamp(1.0 - 0.6*c.x*c.x, 0.0, 1.0);
    col -= (1.0-smoothstep( 0.05, 0.06, c.x));                   
    fragmentcolor += vec4( col, 0.0 );
}
/************VoronoiDiagram**************/
void main(void)
{
	if(useTessllation)
	{
		if(objType == 2)//WATER
		{
			vec4 lp = ModelViewMatrix * vec4(light_position,1.0);
			vec3 light_vector = normalize(light_position.xyz - vpos.xyz);
			//vec4 normalmapcolor = texture(normalmap,uv);
			//vec3 normal = vec3(normalmapcolor.r * 2 - 1,normalmapcolor.b,normalmapcolor.g * 2 - 1);
			//mat3 normalmatrix = transpose(inverse(mat3(ModelViewMatrix)));
			vec4 black = vec4(0,0,0,1);						
			//vec3 normal = normalize((ModelViewMatrix *vec4(nor,0))).xyz;
			vec3 normal = nor;			
			vec3 camera = vec3(ModelViewMatrix * vec4(camerapos,1.0));			
			vec3 cameradir = normalize(camera.xyz - vpos.xyz);
			float dv = dot(light_vector,normal);
			float diffuse = max(0.0,dv);

			vec3 light_reflect = normalize(reflect(-light_vector,normal));
			//vec3 halfvector = normalize(light_vector+cameradir);
			float sv = dot(light_reflect,normal);
			float specular = pow(max(0.0,sv),shininess);
			//vec4 light_color = vec4(min(c * color_ambient,vec3(1.0)) + diffuse * color_diffuse + specular*color_specular,0);						
			vec3 RefractVec = refract(cameradir,normal, eta);
    		vec3 ReflectVec = reflect(cameradir, normal);	
		
			vec4 RefractColor = texture(texcube,RefractVec);
			vec4 ReflectColor = texture(texcube,ReflectVec);			
			vec4 water_color = texture2D(tex,uv);//Water
			fragmentcolor = water_color;
			fragmentcolor = mix(fragmentcolor,RefractColor,0.4);
			//fragmentcolor = mix(fragmentcolor,ReflectColor,0.4);
			fragmentcolor += vec4(0,0,0.1,0);
			if(effect == 11)
				Abstraction();
			black += fragmentcolor * vec4(color_ambient,1);
			black += fragmentcolor * diffuse * vec4(color_diffuse,1);
			black += fragmentcolor * specular * vec4(color_specular,1);									
			if(alpha !=1)
				black.a = alpha;			
			fragmentcolor = black;
		}
		else//Terrain
		{
			vec4 lp = ModelViewMatrix * vec4(light_position,1.0);
			vec3 light_vector = normalize(light_position.xyz - vpos.xyz);
			vec4 black = vec4(0,0,0,1);	
			//vec3 normal = normalize((ModelViewMatrix *vec4(nor,0))).xyz;
			vec3 normal = nor;
			float dv = dot(light_vector,normal);
			float diffuse = max(0.0,dv);
			vec3 light_reflect = normalize(reflect(-light_vector,normal));
			float sv = dot(light_reflect,normal);
			float specular = pow(max(0.0,sv),shininess);
			//vec4 light_color = vec4(min(c * color_ambient,vec3(1.0)) + diffuse * color_diffuse + specular*color_specular,0);
			vec4 color = texture2D(tex,uv);
			fragmentcolor = color;
			if(effect == 11)
				Abstraction();
			black += fragmentcolor * vec4(color_ambient,1);
			black += fragmentcolor * diffuse * vec4(color_diffuse,1);
			black += fragmentcolor * specular * vec4(color_specular,1);
			if(alpha !=1)
				black.a = alpha;
			fragmentcolor = black;
		}
	}else
	{
		if(objType == 3)//Model reflect environment map
		{
			vec3 normal = vs_normal;
			vec3 camera = vec3(ModelViewMatrix * vec4(camerapos,1.0));
			vec3 cameradir = normalize(camera.xyz - vec3(ModelViewMatrix * vec4(texturecoord3d,1.0)));			
			float dv = dot(light_vector,normal);
			float diffuse = max(0.0,dv);

			vec3 light_reflect = normalize(reflect(-light_vector,normal));			
			float sv = dot(light_reflect,normal);			
			float specular = pow(max(0.0,sv),shininess);				
			vec3 RefractVec = refract(cameradir,normal, eta);
    		vec3 ReflectVec = reflect(cameradir,normal);	
			RefractVec = vec3(RefractVec.x,-RefractVec.y,-RefractVec.z);
			ReflectVec = vec3(ReflectVec.x,-ReflectVec.y,-ReflectVec.z);			
			vec4 RefractColor = texture(texcube,RefractVec);
			vec4 ReflectColor = texture(texcube,ReflectVec);			
												
			vec4 color = texture2D(tex,texturecoord);
			fragmentcolor = color;
			if(effect == 11)
				Abstraction();
			vec4 black = vec4(0,0,0,1);
			fragmentcolor = mix(fragmentcolor,RefractColor,0.7);
			fragmentcolor = mix(fragmentcolor,ReflectColor,0.7);
			
			black += fragmentcolor * vec4(color_ambient,1);
			black += fragmentcolor * diffuse * vec4(color_diffuse,1);
			black += fragmentcolor * specular * vec4(color_specular,1);
			if(alpha !=1)
				black.a = alpha;			
			fragmentcolor = black;
		}
		else if(objType == 0)
		{
			vec3 normal = vs_normal;		
			vec4 black = vec4(0,0,0,1);
			float dv = dot(light_vector,normal);
			float diffuse = max(0.0,dv);
			vec3 light_reflect = normalize(reflect(-light_vector,normal));		
			float sv = dot(light_reflect,normal);			
			float specular = pow(max(0.0,sv),shininess);																										
			vec4 color = texture2D(tex,texturecoord);
			if(color.a < 0.1)
				discard;
			fragmentcolor = color;
			if(effect == 11)
				Abstraction();
			black += fragmentcolor * vec4(color_ambient,1);
			black += fragmentcolor * diffuse * vec4(color_diffuse,1);
			black += fragmentcolor * specular * vec4(color_specular,1);
			if(alpha !=1)
				black.a = alpha;
			fragmentcolor = black;
		}
		else if(objType == 1)//Skybox
		{
			fragmentcolor = texture(texcube,texturecoord3d);
		}
	}
	switch(effect)
    {
        case(0)://Normal
        {						
            break;
        }
        case(1)://Gray
        {
			Grayscale();
            break;
        }
		case(2)://Snow
		{
			snow();			
            break;
		}
        case(3)://Nebula smoke
		{
			NebulaSmoke();
            break;
		}
        case(4)://Phosgene
		{
			Phosgene();
            break;
		}
        case(5)://Purple halo
		{
		    PurpleHalo();
            break;
		}
        case(6)://Dance floor
		{
		    DanceFloor();
            break;
		}
		case(7)://Threshold dithering
		{
		    ThresholdDither();
            break;
		}
		case(8)://Text speak
		{
		    Textspeak();
            break;
		}
		case(9)://Voronoi diagram
		{
		    VoronoiDiagram();
            break;
		}
		case(10)://Gold
		{
			if(!useTessllation)
				gold_particle();
            break;
		}
    }
}
