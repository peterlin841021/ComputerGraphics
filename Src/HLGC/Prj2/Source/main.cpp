#include "..\include\Common.h"
#define MENU_EXIT   4

using namespace glm;
using namespace std;

GLuint sp;
const unsigned int interval = 10;
GLuint vao, vvbo;
const size_t defalut_w = 800;
const size_t defalut_h = 600;

void Render(glm::mat4 pm, glm::mat4 mm,vector<vec3> pos,vector<vec2> uv, vector<int> buffersize, clock_t time, int effect);
GLuint generateTexture(const char *image);

struct Uniform
{
	GLuint mv;
	GLuint pm;
	GLuint time;
	GLuint effect;
	GLuint tex;
};
struct Character 
{
	string charactername;
	mat4 modelview;
	mat4 projection;
	vector<vector<vec2>> action;	
	GLuint textureid;
	float xpos, ypos;
	float angley;
	Character(string name,mat4 mv,mat4 pm, vector<vector<vec2>> uvs)
	{
		charactername = name;
		modelview = mv;
		projection = pm;
		action = uvs;
		xpos = 0;
		ypos = 0;
		angley = 0;
	}
};
Uniform *uniform;
vector<Character*> characters;
vector<vector<vec2>> generate_ani_uv(float origin_w, float origin_h,size_t wpart, size_t hpart)
{
	vector<vector<vec2>> output;
	float xstart = 0.0,ystart = 1.0;
	float xoffset = (origin_w / wpart) / origin_w;
	float yoffset = (origin_h / hpart) / origin_h;
	for (size_t i = 0; i < hpart; i++)
	{
		float x = 0;		
		for (size_t j = 0; j < wpart; j++)
		{
			vector<vec2> v;
			xstart += xoffset;
			v.push_back(vec2(x, ystart));
			v.push_back(vec2(xstart, ystart));
			v.push_back(vec2(xstart, ystart - yoffset));
			v.push_back(vec2(xstart, ystart - yoffset));
			v.push_back(vec2(x, ystart - yoffset));
			v.push_back(vec2(x, ystart));
			x = xstart;
			output.push_back(v);
		}
		ystart -= yoffset;
		xstart = 0;	
	}
	return output;
}
int index = 0;
int max_index = 0;
float movex = 0;
float enemyx = 0.1f;
void init_shader()
{
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
}
void My_Init()
{
	vector<vector<vec2>> m_uv, k_uv,castle_uv,p_uv;
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_uv = generate_ani_uv(14400, 200, 72, 1);//miku	
	k_uv = generate_ani_uv(5304, 395, 24, 1);//kizuna
	castle_uv = generate_ani_uv(679, 376, 4, 1);
	p_uv = generate_ani_uv(384, 384, 4, 4);
	init_shader();
	//Uniform variables
	uniform = new Uniform();
	uniform->pm = glGetUniformLocation(sp, "pm");
	uniform->mv = glGetUniformLocation(sp, "mm");
	uniform->effect = glGetUniformLocation(sp, "effect");
	uniform->time = glGetUniformLocation(sp, "time");
	float sc = 0.2;
	mat4 pm(1.0);
	mat4 identity(1.0);
	mat4 mv(1.0);
	mv * translate(identity, vec3(0,2,0));
	mv *= scale(identity, vec3(sc, sc, 1));
	Character* cs[4] = 
	{ 
		new Character("Castle",mat4(1.0)*scale(identity,vec3(1,1,1)),pm,castle_uv),
		new Character("Miku",mv,pm,m_uv),
		new Character("Kizuna",mv,pm,k_uv),
		new Character("Princess",mv,pm,p_uv)
	};

	cs[0]->textureid = generateTexture("castleL.png");
	characters.push_back(cs[0]);
	cs[1]->textureid = generateTexture("miku_75.png");
	characters.push_back(cs[1]);
	cs[2]->textureid = generateTexture("kizuna_24.png");
	characters.push_back(cs[2]);
	cs[3]->textureid = generateTexture("princess.png");
	characters.push_back(cs[3]);

	for (size_t i = 0; i < characters.size(); i++)
	{
		if (max_index < characters[i]->action.size())
			max_index = characters[i]->action.size();
	}
}
GLuint generateTexture(const char *image)
{
	GLuint id = characters.size();
	TextureData *image_data = &(Load_png(image));
	if (!image_data->data)
	{
		printf("Read image fail!\n");
		system("pause");
		return 0;
	}
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, image_data->width, image_data->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return id;
}

void Render(glm::mat4 pm, glm::mat4 mm, vector<vec3> pos, vector<vec2> uv, vector<int> buffersize, clock_t time,int effect)
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
void resetaction(unsigned char key,int x,int y)
{
	switch (key)
	{
	case 'a':
	case 'A':
		//index = 0;
		break;
	case 'd':
	case 'D':
		//index = 0;
		break;
	default:
		
		break;
	}
}
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1, 1, 1, 1);
	
	glUseProgram(sp);
	//Two triangles
	vector<vec3> pos;
	pos.push_back(vec3(-1,1,0));
	pos.push_back(vec3(1, 1, 0));
	pos.push_back(vec3(1, -1, 0));

	pos.push_back(vec3(1, -1, 0));
	pos.push_back(vec3(-1, -1, 0));
	pos.push_back(vec3(-1, 1, 0));
	vector<int> buffer_size;
	buffer_size.push_back(6);
	buffer_size.push_back(6);
	//Time
	clock_t time = clock();
	
	if (characters[2]->xpos > 5.0f)
	{
		enemyx = -0.1f;		
	}		
	else if (characters[2]->xpos < -5.0)
	{
		enemyx = 0.1f;
	}
	characters[2]->xpos += enemyx;
	characters[2]->modelview *= translate(mat4(1.0), vec3(enemyx, 0, 0));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthFunc(GL_LEQUAL);
	for (size_t i = 0; i < characters.size(); i++)
	{		
		glActiveTexture(characters[i]->textureid);
		glBindTexture(GL_TEXTURE_2D, characters[i]->textureid);
		if (i == 0 || i == characters.size() - 1)
			Render(characters[i]->modelview, characters[i]->projection, pos, characters[i]->action[index % characters[i]->action.size()], buffer_size, time, 0);
	}
	
	glDisable(GL_BLEND);
	glutSwapBuffers();
}
//Call to resize the window
void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}
//Kayboard input
void keyboardevent(unsigned char key,int x,int y) 
{	
	
	switch (key)
	{
	case 'w':
	case 'W':		
		break;
	case 'a':
	case 'A':
		movex = -0.1f;
		//characters[0]->xpos -= movex;
		characters[2]->modelview *= translate(mat4(1.0), vec3(movex, 0, 0));
		index++;
		break;
	case 's':
	case 'S':		
		break;
	case 'd':
	case 'D':
		movex = 0.1f;
		//characters[0]->xpos += movex;
		characters[2]->modelview *= translate(mat4(1.0), vec3(movex, 0, 0));
		index++;
		break;
	case '+':
		
		break;
	case '-':
		
		break;
	default:		
		break;
	}
	
}
//Timer event
void My_Timer(int val)
{	
	glutPostRedisplay();
	glutTimerFunc(interval, My_Timer, val);
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
	glutKeyboardFunc(keyboardevent);
	glutKeyboardUpFunc(resetaction);
	
	glutTimerFunc(interval, My_Timer, 0);
	// Enter main event loop.
	glutMainLoop();
	return 0;
}