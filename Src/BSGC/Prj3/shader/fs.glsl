#version 430 core
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
			fragmentcolor = vec4(fcolor,1.0);
            break;
        }
        case(1)://Draw texture
        case(2)://Draw skybox
        {
			vec4 color = texture2D(tex,texturecoord);
            if(alpha !=1)
			    color.a = alpha;
			fragmentcolor = color;
            break;
        }        
    }			
	// vec4 RefractColor = textureCube(box,RefractVec);
	// vec4 ReflectColor = textureCube(box,ReflectVec);
	//color = mix(mix(water,light_color,0.2),sky,0.4);
	//color = mix(water,light_color,0.2);
}
