#include "..\include\Common.h"
#define MENU_EXIT   4

using namespace glm;
using namespace std;

GLuint sp;
int shader_now = 0;
GLuint hawk_texture;
GLint Shader_now_Loc;
GLuint vao, vvbo;
int defalut_w = 800;
int defalut_h = 600;
vector<TextureData*>textures;
void Render(glm::mat4 pm, glm::mat4 mm,vector<vec3> pos,vector<vec2> uv, vector<int> buffersize,float time, int drawtype, int effect);
struct Uniform
{
	GLuint mv;
	GLuint pm;
	GLuint time;
	GLuint effect;
};
Uniform *uniform;
void My_Init()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	uniform = new Uniform();
	//Initialize shaders	
	sp = glCreateProgram();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	char** vsSource = LoadShaderSource("./vs.glsl");
	char** fsSource = LoadShaderSource("./fs.glsl");
	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);

	glCompileShader(vs);
	glCompileShader(fs);
	ShaderLog(vs);
	ShaderLog(fs);
	//Attach Shader to program
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);
	
	glGenBuffers(1, &vvbo);	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uniform->pm = glGetUniformLocation(sp, "pm");
	uniform->mv = glGetUniformLocation(sp, "mm");
	uniform->effect = glGetUniformLocation(sp, "effect");
	uniform->time = glGetUniformLocation(sp, "time");
	//printf("Uniform:%d,%d,%d,%d\n", uniform->mv, uniform->pm, uniform->time, uniform->effect);
	TextureData *tdata = &(Load_png("davis.jpg"));
	
	if (!tdata->data)
	{
		printf("Read image fail!");
		system("pause");
	}
	else
	{
		textures.push_back(tdata);
	}
	for (size_t i = 0; i < textures.size(); i++)
	{
		glGenTextures(1, &i);
		glBindTexture(GL_TEXTURE_2D, i);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tdata->width, tdata->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}
void Render(glm::mat4 pm, glm::mat4 mm, vector<vec3> pos, vector<vec2> uv, vector<int> buffersize,float time,int drawtype,int effect)
{	
	glUseProgram(sp);
	glBindVertexArray(vao);	
	float mv_matrix[16] = { 0 };
	float pm_matrix[16] = { 0 };
	int pos_size = buffersize[0];
	int color_size = buffersize[1];
	int total_size = 0;

	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			mv_matrix[i * 4 + j] = mm[i][j];
		}
	}
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			pm_matrix[i * 4 + j] = pm[i][j];
		}
	}
	for (size_t i = 0; i < buffersize.size(); i++)
	{
		total_size += buffersize[i] * 3;
	}

	float *v = new float[total_size];
	int vec_index = 0;
	for (int i = 0; i < pos_size; i++)
	{
		v[vec_index] = pos[i].x;
		vec_index++;
		v[vec_index] = pos[i].y;
		vec_index++;
		v[vec_index] = pos[i].z;
		vec_index++;
	}	
	for (int i = 0 ; i < color_size; i ++)
	{
		v[vec_index] = uv[i].x;
		vec_index++;
		v[vec_index] = uv[i].y;
		vec_index++;
	}
	//printf("Uniform:%d,%d,%d,%d\n", uniform->mv, uniform->pm, uniform->time, uniform->effect);
	glUniformMatrix4fv(uniform->mv,1, GL_FALSE, mv_matrix);
	glUniformMatrix4fv(uniform->pm, 1, GL_FALSE, pm_matrix);
	glUniform1i(uniform->effect, effect);
	glUniform1f(uniform->time, time);

	glBindBuffer(GL_ARRAY_BUFFER, vvbo);
	glBufferData(GL_ARRAY_BUFFER, total_size * sizeof(float),v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0,0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,0, (void*)(pos_size * 3 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0,pos.size());
}
// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	
	glUseProgram(sp);
	vector<vec3> pos;
	pos.push_back(vec3(-1,1,0));
	pos.push_back(vec3(1, 1, 0));
	pos.push_back(vec3(1, -1, 0));

	pos.push_back(vec3(1, -1, 0));
	pos.push_back(vec3(-1, -1, 0));
	pos.push_back(vec3(-1, 1, 0));

	vector<vec2> uv;
	uv.push_back(vec2(0,1));
	uv.push_back(vec2(1, 1));
	uv.push_back(vec2(1, 0));

	uv.push_back(vec2(1, 0));
	uv.push_back(vec2(0,0));
	uv.push_back(vec2(0,1));

	vector<int> buffer_size;
	buffer_size.push_back(6);
	buffer_size.push_back(6);
	float t = clock();
	
	Render(mat4(1.0), mat4(1.0), pos,uv,buffer_size,(t),1,0);
	
	glutSwapBuffers();
}

//Call to resize the window
void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

//Timer event
void My_Timer(int val)
{
	glutPostRedisplay();
	glutTimerFunc(16, My_Timer, val);
}

//Menu event
void My_Menu(int id)
{
	switch (id)
	{
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.	
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(defalut_w, defalut_h);
	glutCreateWindow("Game"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif	
	DumpInfo();
	//Call custom initialize function
	My_Init();

	//Define Menu
	int menu_main = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//Register GLUT callback functions
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutTimerFunc(16, My_Timer, 0);	
	// Enter main event loop.
	glutMainLoop();
	return 0;
}