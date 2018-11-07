#version 430 core
in vec3 vs_worldpos;
in vec3 vs_normal;
in vec2 vUV;
in vec3 RefractVec;
in vec3 ReflectVec;
layout(location = 0) out vec4 color;
uniform vec4 color_ambient = vec4(0.1,0.2,0.5,1.0);
uniform vec4 color_diffuse = vec4(0.2,0.3,0.6,1.0);
uniform vec4 color_specular = vec4(1.0,1.0,1.0,1.0);
uniform vec4 Color = vec4(0.6,0.6,0.6,0.1);
uniform float shininess = 77.f;
uniform vec3 light_position = vec3(30.f,50.f,560.f);
uniform sampler2D Sky;
uniform sampler2D Texture;
uniform samplerCube box;
void main(void)
{	
	vec3 light_direction = normalize(light_position - vs_worldpos);
	vec3 normal = normalize(vs_normal);
	vec3 half_vector = normalize(normalize(light_direction)+normalize(vs_worldpos));
	float dv = normal.x*light_direction.x+normal.y*light_direction.y+normal.z*light_direction.z;
	float diffuse = max(0.0,dv);
	float sv = vs_normal.x*half_vector.x+vs_normal.y*half_vector.y+vs_normal.z*half_vector.z;
	float specular = pow(max(0.0,sv),shininess);
	vec4 light_color = min(Color * color_ambient,vec4(1.0))+diffuse*color_diffuse+specular*color_specular;
	vec4 water = texture2D(Texture,vUV);		
	// vec4 RefractColor = textureCube(box,RefractVec);
	// vec4 ReflectColor = textureCube(box,ReflectVec);
	//color = mix(mix(water,light_color,0.2),sky,0.4);
	color = mix(water,light_color,0.2);	
	color.a =0.45;
}
