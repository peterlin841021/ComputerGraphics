#version 430 core
const vec2 iResolution = vec2(800., 800.);
// in vec3 vs_worldpos;
// in vec3 vs_normal;
in vec2 texturecoord;
in vec3 fcolor;
// in vec3 RefractVec;
// in vec3 ReflectVec;
out vec4 fragmentcolor;
// uniform vec4 color_ambient = vec4(0.1,0.2,0.5,1.0);
// uniform vec4 color_diffuse = vec4(0.2,0.3,0.6,1.0);
// uniform vec4 color_specular = vec4(1.0,1.0,1.0,1.0);
// uniform vec4 Color = vec4(0.6,0.6,0.6,0.1);
// uniform float shininess = 77.f;
// uniform vec3 light_position = vec3(30.f,50.f,560.f);
uniform int mode;
uniform float alpha;
uniform sampler2D tex;
uniform float time;
float rnd(float x) {  return fract(sin(dot(vec2(x + 47.49, 38.2467 / (x + 2.3)),vec2(12.9898, 78.233))) * (43758.5453));}
/*********Snow**********/
#define _SnowflakeAmount 300  
#define _BlizardFactor 0.25          

float drawCircle(vec2 uv, vec2 center, float radius)
{
   return 1.0 - smoothstep(0.0, radius, length(uv - center));
}

void snow()
{
   vec2 uv =  gl_FragCoord.xy  / iResolution.x;
   vec3 color = texture(tex,texturecoord).rgb;  
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
void main(void)
{	
	// vec3 light_direction = normalize(light_position - vs_worldpos);
	// vec3 normal = normalize(vs_normal);
	// vec3 half_vector = normalize(normalize(light_direction)+normalize(vs_worldpos));
	// float dv = normal.x*light_direction.x+normal.y*light_direction.y+normal.z*light_direction.z;
	// float diffuse = max(0.0,dv);
	// float sv = vs_normal.x*half_vector.x+vs_normal.y*half_vector.y+vs_normal.z*half_vector.z;
	// float specular = pow(max(0.0,sv),shininess);
	// vec4 light_color = min(Color * color_ambient,vec4(1.0))+diffuse*color_diffuse+specular*color_specular;	
	switch(mode)
    {
        case(0)://Draw origin color
        {
			// if(alpha !=1)			    
			// 	fragmentcolor = vec4(fcolor,alpha);
			// else
			// 	fragmentcolor = vec4(fcolor,1.0);			
			gold_particle();
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
		}
    }			
	// vec4 RefractColor = textureCube(box,RefractVec);
	// vec4 ReflectColor = textureCube(box,ReflectVec);
	//color = mix(mix(water,light_color,0.2),sky,0.4);
	//color = mix(water,light_color,0.2);
}
