#version 430 core
uniform sampler2D Texture;
uniform int mode;
uniform float time;
in vec2 vUV;
in vec4 colors;
out vec4 fColor;
const vec2 iResolution = vec2(600., 600.);
in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord; //座標
} vertexData;
float rnd(float x) {  return fract(sin(dot(vec2(x + 47.49, 38.2467 / (x + 2.3)),vec2(12.9898, 78.233))) * (43758.5453));}

/*-------SNOW----------*/
#define _SnowflakeAmount 300  
#define _BlizardFactor 0.25          
float drawCircle(vec2 uv, vec2 center, float radius)
{
   return 1.0 - smoothstep(0.0, radius, length(uv - center));
}
void snow_texture()
{
   vec2 uv =  gl_FragCoord.xy  /iResolution.x;
   vec3 color = texture(Texture,vUV).rgb;  
   fColor = vec4(color, 1.0);
   float j;   
   for (int i = 0; i < _SnowflakeAmount; i++)
   {
      j = float(i);      
      float speed = 0.3 + rnd(cos(j)) * (0.7 + 0.5 * cos(j / (float(_SnowflakeAmount) * 0.25)));           
      vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(time*0.001 + sin(j)) + 1.0,mod(rnd(j) - speed * (time * 0.001 * (0.1 + _BlizardFactor)),0.95));
      fColor += vec4(0.9 * drawCircle(uv, center , 0.001 + speed * 0.008 ));	  
   }   
}
void snow_wall()
{
   vec2 uv =  gl_FragCoord.xy  /iResolution.x;
   //vec3 color = mix(colors.rgb,texture(Texture,vUV).rgb,0.4);   
   //fColor = vec4(color, 1.0);
   fColor = colors;
   float j;   
   for (int i = 0; i < _SnowflakeAmount; i++)
   {
      j = float(i);      
      float speed = 0.3 + rnd(cos(j)) * (0.7 + 0.5 * cos(j / (float(_SnowflakeAmount) * 0.25)));           
      vec2 center = vec2((-0.25 + uv.y) * _BlizardFactor + rnd(j) + 0.1 * cos(time*0.001 + sin(j)) + 1.0,mod(rnd(j) - speed * (time * 0.001 * (0.1 + _BlizardFactor)),0.95));
      fColor += vec4(0.9 * drawCircle(uv, center , 0.001 + speed * 0.008 ));	  
   }   
}
/*-----------------*/
void main()
{
	switch(mode)
	{
		case 0:
		//fColor = colors;
		snow_wall();
		break;
		case 1:
		//fColor = texture(Texture,vUV);
		snow_texture();
		break;
		case 2:
		fColor = colors;		
		break;
	}	
}