#version 430 core
const vec2 iResolution = vec2(800., 800.);
uniform vec3 camerapos;
in vec2 texturecoord;
in vec3 texturecoord3d;
in vec3 fcolor;
in vec2 tc;
in vec3 vpos;
in vec3 vs_normal;
in vec3 nor;
in vec2 uv;
out vec4 fragmentcolor;
vec3 color_diffuse = vec3(0.3,0.3,0.3);
vec3 color_specular = vec3(1.0,1.0,1.0);
//vec3 color_ambient = vec3(0.5,0.5,1.0);
vec3 color_ambient = vec3(0.7,0.7,1.0);
vec3 light_position = vec3(30.f,50.f,560.f);
vec3 c = vec3(0.6,0.6,0.6);
float shininess = 20.f;
float reflectivity = 0.6f;
float eta = 0.66;
uniform int effect;
uniform float alpha;
uniform sampler2D tex;
uniform sampler2D normalmap;
uniform samplerCube texcube;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform float time;

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
	//vec3 texColor = texture(tex,texturecoord).rgb;
    vec3 texColor = fcolor;
	fragmentcolor = vec4(texColor, 1.0);	
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
	vec3 texColor = texture(tex,texturecoord).rgb;	
	fragmentcolor = vec4(texColor, 1.0);
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
   vec3 color = texture(texcube,texturecoord3d).rgb;  
   fragmentcolor = vec4(color, 1.0);  
   
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
   //vec3 color = texture(tex,vertexData.texcoord).rgb;
   
   fragmentcolor = vec4(fcolor, alpha);
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
    vec3 texColor = texture(tex,texturecoord).rgb;	
	fragmentcolor = vec4(texColor, 1.0);	
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
	vec3 texColor = texture(tex,texturecoord).rgb;	
	fragmentcolor = vec4(texColor, 1.0);
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
void main(void)
{
	switch(effect)
    {
        case(0)://Origin color
        {			
            if(alpha !=1)			    
				fragmentcolor = vec4(fcolor,alpha);
			else
				fragmentcolor = vec4(fcolor,1.0);
            break;
        }
        case(1)://Draw texture        
        {
			vec4 color = texture2D(tex,texturecoord);
            if(alpha !=1)
			    color.a = alpha;
			fragmentcolor = color;			
            break;
        }
		case(2)://Draw skybox
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
		case(7)://Tessellation
		{
			//vec3 light_direction = normalize(vpos.xyz - vec3(ModelViewMatrix * vec4(light_position,1.0)));
			vec3 light_direction = normalize(vec3(ModelViewMatrix * vec4(light_position,1.0)) - vpos.xyz);
			vec4 normalmapcolor = texture(normalmap,texturecoord);
			//vec3 normal = vec3(normalmapcolor.r * 2 - 1,normalmapcolor.b,normalmapcolor.g * 2 - 1);
			vec3 normal = nor;
			//mat3 normalmatrix = transpose(inverse(mat3(ModelViewMatrix)));
			//normal = normalize(normalmatrix * normal);

			vec3 camera = vec3(ModelViewMatrix * vec4(camerapos,1.0));			
			vec3 cameradir = normalize(camera.xyz - vpos.xyz);
			float dv = dot(light_direction,normal);
			float diffuse = max(0.0,dv);

			vec3 halfvector = normalize(light_direction+cameradir);
			float sv = dot(halfvector,normal);
			float specular = pow(max(0.0,sv),shininess);
			vec3 light_color = min(c * color_ambient,vec3(1.0)) + diffuse * color_diffuse + specular*color_specular;			
						
			vec3 RefractVec = refract(cameradir,normal, eta);
    		vec3 ReflectVec = reflect(cameradir, normal);	
			RefractVec = vec3(RefractVec.x,-RefractVec.y,-RefractVec.z);
			ReflectVec = vec3(ReflectVec.x,-ReflectVec.y,-ReflectVec.z);
			vec4 RefractColor = texture(texcube,RefractVec);
			vec4 ReflectColor = texture(texcube,ReflectVec);			
			ReflectColor = mix(RefractColor,ReflectColor,0.5);

			vec4 color = texture2D(tex,texturecoord);//Water
			//color *= vec4(light_color,0.6);
			//color = mix(color,vec4(light_color,1.0),0.3);
			//color = mix(color,ReflectColor,0.5);
			
			//color = color * ReflectColor;
			//color = RefractColor;
			if(alpha !=1)
				color.a = 1;			
			fragmentcolor = color;
            break;
		}
		case(8)://OuO
		{
		    vec3 light_direction = normalize(vec3(ModelViewMatrix * vec4(light_position,1.0)) - texturecoord3d.xyz);
			
			vec3 normal = vs_normal;
			//mat3 normalmatrix = transpose(inverse(mat3(ModelViewMatrix)));			
			//normal = normalize(normalmatrix * normal);

			vec3 camera = vec3(ModelViewMatrix * vec4(camerapos,1.0));			
			vec3 cameradir = normalize(camera.xyz - vec3(ModelViewMatrix * vec4(texturecoord3d,1.0)));			
			float dv = dot(light_direction,vs_normal);
			float diffuse = max(0.0,dv);

			vec3 halfvector = normalize(light_direction+cameradir);
			float sv = dot(halfvector,vs_normal);			
			float specular = pow(max(0.0,sv),shininess);
			vec3 light_color = min(c * color_ambient,vec3(1.0)) + diffuse * color_diffuse + specular*color_specular;			
						
			vec3 RefractVec = refract(cameradir,vs_normal, eta);
    		vec3 ReflectVec = reflect(cameradir,vs_normal);	
			RefractVec = vec3(RefractVec.x,-RefractVec.y,-RefractVec.z);
			ReflectVec = vec3(ReflectVec.x,-ReflectVec.y,-ReflectVec.z);

			vec4 RefractColor = texture(texcube,RefractVec);
			vec4 ReflectColor = texture(texcube,ReflectVec);			
			ReflectColor = mix(RefractColor,ReflectColor,0.5);

			vec4 color = texture2D(tex,texturecoord);
			color *= vec4(light_color,1.0);			
			//color = mix(color,vec4(light_color,1.0),0.3);
			color = mix(color,ReflectColor,0.5);			
			if(alpha !=1)
				color.a = alpha;			
			fragmentcolor = color;            
            break;
		}
    }
}
