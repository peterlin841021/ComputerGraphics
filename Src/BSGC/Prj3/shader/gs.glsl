#version 430 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 teposition[3];
in vec3 tepatchdistance[3];

out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;

void main()
{
	
}