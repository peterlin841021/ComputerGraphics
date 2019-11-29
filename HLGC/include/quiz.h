#include<iostream>
#include<vector>
#include<stack>
#include<glew.h>
#include<glut.h>
#include<cmath>
#include<fstream>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>

using namespace std;
using namespace glm;

void shader_init();
GLuint mv_location, proj_location;
void all_display();
int window_width = 800;
int window_height = 600;
stack<mat4> matrix_stack;
mat4 modelviewmatrix, projectionmatrix;
GLuint program, triangle_vao, buffer;

GLchar* readShaderSource(string path);
void drawTriangle(float *vertex, float *color, int vertex_size, int color_size, mat4 mm, mat4 pm);
void timer(int v);


